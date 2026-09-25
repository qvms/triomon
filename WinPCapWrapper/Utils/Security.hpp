#pragma once
#include <vector>
#include <QString>
#include <QDir>
#include <QFileInfo>

namespace NetworkWrapper {
namespace Utils {

class Security {
public:
    static bool DllHijackingAttempted(const std::vector<QString>& paths, const std::vector<QString>& dllFileNames, QString& hijackedPath) {
        for (const QString& path : paths) {
            QString trimmedPath = path;
            if (trimmedPath.endsWith(QDir::separator())) {
                trimmedPath.chop(1);
            }
            if (!trimmedPath.endsWith("system32", Qt::CaseInsensitive)) {
                for (const QString& dll : dllFileNames) {
                    if (DllHijackingAttempted(path, dll)) {
                        hijackedPath = path + QDir::separator() + dll;
                        return true;
                    }
                }
            }
        }
        hijackedPath = "";
        return false;
    }

    static bool DllHijackingAttempted(const QString& path, const QString& dllFileName) {
        QDir dir(path);
        QFileInfo fi(dir.absolutePath() + QDir::separator() + dllFileName);
        return fi.exists() && fi.isFile();
    }
};

}
}
