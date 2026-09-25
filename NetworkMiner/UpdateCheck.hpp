#pragma once
#include <QString>
#include <QDateTime>
#include <functional>

namespace NetworkMiner {

class UpdateCheck {
public:
    static QString CachedLocalVersionCode;
    static std::function<void()> CachedDownloadButtonAction;
    static QString HelpText;

    static void ShowNewVersionFormIfAvailableAsync(void* parentForm, const QString& localVersion, bool showMessageBoxIfNoUpdate = false);
    static void ShowNewVersionFormIfAvailableAsync(void* parentForm, const QString& localVersion, const QString& productCode, const std::vector<uint8_t>& extra, bool showMessageBoxIfNoUpdate = false);

private:
    static bool TryGetTimestampFile(QString& timestampFile);
    static QDateTime GetLastUpdateCheck();
    static void UpdateLastUpdateCheck();

public:
    UpdateCheck(const QString& newVersion, const QString& releasePost, const QString& downloadUrl);
    std::function<void()> DownloadButtonAction;
};

} // namespace NetworkMiner
