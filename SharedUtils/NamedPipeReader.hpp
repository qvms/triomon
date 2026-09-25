#pragma once

#include <QString>
#include <QProcess>
#include <QFileInfo>
#include <QObject>
#include <memory>
#include <tuple>

namespace SharedUtils {

class NamedPipeReader : public QObject {
    Q_OBJECT
public:
    enum class State {
        ProcessNotStarted,
        WaitingForFile,
        WritingToFile,
        ProcessStoped,
        Stopped
    };

private:
    std::shared_ptr<QProcess> process;
    QString outputFilePath;
    std::unique_ptr<QFileInfo> outputFileInfo;
    State state;
    bool disposedValue;
    long long bytesReadTotal;
    uint16_t updateTimeMilliseconds;

    long long GetNewBytesRead();

public:
    NamedPipeReader(const QString& pipeName, const QString& outputFilePath, bool requireAdmin, const QString& bpf = "", uint16_t updateTimeMilliseconds = 500, std::shared_ptr<std::tuple<QString, QString, QString>> runAs = nullptr, QObject* parent = nullptr);
    ~NamedPipeReader();

    long long BytesReadTotal() const { return bytesReadTotal; }
    bool HasStopped() const { return state == State::Stopped; }
    uint16_t UpdateTimeMilliseconds() const { return updateTimeMilliseconds; }

    // Mocking async read functions as regular methods for C++ adaptation without full C++20 coroutines
    long long ReadAll(int idleTimeoutMilliseconds = 60000);
    long long Read(int idleTimeoutMilliseconds = 60000);

    void Stop(bool killProcess = false);
    void Dispose();

protected:
    virtual void Dispose(bool disposing);
};

} // namespace SharedUtils
