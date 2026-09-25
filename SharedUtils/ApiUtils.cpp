#include "ApiUtils.hpp"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QStringList>

namespace SharedUtils {

std::string ApiUtils::GetLatestVersion(const std::string& productCode, std::string& releasePost, std::string& downloadUrl) {
    QString requestURL = "https://www.netresec.com/updatecheck.ashx?l=" + QUrl::toPercentEncoding(QString::fromStdString(productCode));
    
    QNetworkAccessManager manager;
    QNetworkRequest request(requestURL);
    // request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    QNetworkReply* reply = manager.get(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    std::string versionString = "";
    if (reply->error() == QNetworkReply::NoError) {
        QString response = QString::fromUtf8(reply->readAll());
        QStringList lines = response.split('\n');
        if (lines.size() >= 3) {
            versionString = lines[0].trimmed().toStdString();
            releasePost = lines[1].trimmed().toStdString();
            downloadUrl = lines[2].trimmed().toStdString();
        }
    }
    reply->deleteLater();
    return versionString;
}

}
