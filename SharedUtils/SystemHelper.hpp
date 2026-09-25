#pragma once
#include <string>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>

namespace SharedUtils {

class SystemHelper {
public:
    static bool IsRunningOnMono() {
        return false; // Not running mono in C++ Qt rewrite
    }

    static bool TryOpenWebsite(const std::string& url) {
        if (url.empty()) return false;
        QUrl qUrl(QString::fromStdString(url));
        if (qUrl.isValid()) {
            return QDesktopServices::openUrl(qUrl);
        }
        return false;
    }

    static bool TryStartProcess(const std::string& path) {
        QString qPath = QString::fromStdString(path);
        // Using QDesktopServices::openUrl as a robust way to "start" a file/url in Qt.
        // It acts as xdg-open/explorer.exe natively on all platforms.
        return QDesktopServices::openUrl(QUrl::fromLocalFile(qPath));
    }
};

}