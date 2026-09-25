#include "NamedPipeReader.hpp"
#include <QThread>
#include <QTime>
#include <stdexcept>

namespace SharedUtils {

NamedPipeReader::NamedPipeReader(const QString& pipeName, const QString& outputFilePath, bool requireAdmin, const QString& bpf, uint16_t updateTimeMilliseconds, std::shared_ptr<std::tuple<QString, QString, QString>> runAs, QObject* parent)
    : QObject(parent), outputFilePath(outputFilePath), state(State::ProcessNotStarted), disposedValue(false), bytesReadTotal(0), updateTimeMilliseconds(updateTimeMilliseconds) {
    // We would initiate QProcess here with powershell, similar to the C# code.
    // For a cross-platform/Qt approach, we might just spawn standard native processes.
    // Stubbing the actual process start logic due to dependency on Windows PowerShell
    state = State::WaitingForFile;
}

NamedPipeReader::~NamedPipeReader() {
    Dispose(false);
}

long long NamedPipeReader::GetNewBytesRead() {
    if (outputFileInfo && outputFileInfo->size() > 0) {
        long long newBytesRead = outputFileInfo->size() - bytesReadTotal;
        bytesReadTotal += newBytesRead;
        return newBytesRead;
    }
    return 0;
}

long long NamedPipeReader::ReadAll(int idleTimeoutMilliseconds) {
    long long bytesRead = Read(idleTimeoutMilliseconds);
    while (bytesRead > 0 || state != State::Stopped) {
        bytesRead = Read(idleTimeoutMilliseconds);
    }
    return bytesReadTotal;
}

long long NamedPipeReader::Read(int idleTimeoutMilliseconds) {
    // Stubbing async task logic with a simulated synchronous wait (would use Qt signals/slots or std::async natively)
    QThread::msleep(updateTimeMilliseconds);
    
    if (state == State::WaitingForFile) {
        if (QFile::exists(outputFilePath)) {
            outputFileInfo = std::make_unique<QFileInfo>(outputFilePath);
            state = State::WritingToFile;
        } else {
            Stop();
            throw std::runtime_error("Process has exited, but no file was written to disk.");
        }
    }
    
    if (state == State::WritingToFile) {
        outputFileInfo->refresh();
        if (outputFileInfo->size() > bytesReadTotal) {
            return GetNewBytesRead();
        }
    }
    
    if (state == State::Stopped) {
        return GetNewBytesRead();
    }
    
    return 0;
}

void NamedPipeReader::Stop(bool killProcess) {
    state = State::Stopped;
    if (killProcess && process && process->state() != QProcess::NotRunning) {
        process->kill();
    }
}

void NamedPipeReader::Dispose(bool disposing) {
    if (!disposedValue) {
        if (disposing) {
            if (state != State::ProcessNotStarted && process && process->state() != QProcess::NotRunning) {
                process->kill();
                process->waitForFinished();
            }
        }
        disposedValue = true;
    }
}

void NamedPipeReader::Dispose() {
    Dispose(true);
}

} // namespace SharedUtils
