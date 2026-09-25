#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <chrono>

namespace SharedUtils {

class ConcurrentLogWriter {
private:
    std::ofstream fs;
    std::mutex writeLock;
    char columnSeparator;
    std::chrono::system_clock::time_point lastFlush;

    void AppendOrCreate(const std::string& path) {
        fs.open(path, std::ios_base::app | std::ios_base::out);
    }

    void ReOpen(const std::string& path) {
        if (fs.is_open()) fs.close();
        AppendOrCreate(path);
    }

public:
    std::chrono::milliseconds AutoFlushInterval;
    std::string NewLine;
    std::string Filename;

    ConcurrentLogWriter(const std::string& path, char columnSeparator = '\t')
        : columnSeparator(columnSeparator), AutoFlushInterval(0), NewLine("\r\n"), Filename(path) {
        AppendOrCreate(path);
        lastFlush = std::chrono::system_clock::now();
    }

    ~ConcurrentLogWriter() {
        Close();
    }

    bool IsTimeToFlush() {
        return AutoFlushInterval.count() > 0 && std::chrono::system_clock::now() - lastFlush > AutoFlushInterval;
    }

    void Flush() {
        std::lock_guard<std::mutex> lock(writeLock);
        if (fs.is_open()) {
            fs.flush();
        } else {
            ReOpen(Filename);
        }
    }

    void WriteLine(const std::string& line) {
        std::string outLine = line + NewLine;
        std::lock_guard<std::mutex> lock(writeLock);
        if (fs.is_open()) {
            fs << outLine;
        } else {
            ReOpen(Filename);
            fs << outLine;
        }

        if (IsTimeToFlush()) {
            lastFlush = std::chrono::system_clock::now();
            fs.flush();
        }
    }

    void Log(const std::vector<std::string>& columns, bool prependTimestamp = true) {
        std::string logLine;
        // Skipping exact timestamp format for simplicity.
        if (prependTimestamp) {
            logLine += "timestamp"; 
        }
        for (const auto& c : columns) {
            logLine += columnSeparator;
            logLine += c;
        }
        WriteLine(logLine);
    }

    void Close() {
        if (fs.is_open()) {
            fs.close();
        }
    }
};

}