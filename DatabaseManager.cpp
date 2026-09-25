#include "DatabaseManager.hpp"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <unistd.h>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

QString DatabaseManager::determineDatabasePath() {
    QString dbName = "triomon.duckdb";
    
    if (isatty(STDOUT_FILENO)) {
        return QDir::currentPath() + QDir::separator() + dbName;
    } else {
        QString homePath = QDir::homePath();
        QDir homeDir(homePath);
        if (!homeDir.exists(".triomon")) {
            homeDir.mkdir(".triomon");
        }
        return homePath + QDir::separator() + ".triomon" + QDir::separator() + dbName;
    }
}

void DatabaseManager::init() {
    QString path = determineDatabasePath();
    qDebug() << "Initializing DuckDB at:" << path;
    
    m_db = std::make_unique<duckdb::DuckDB>(path.toStdString());
    m_conn = std::make_unique<duckdb::Connection>(*m_db);
    
    auto result = m_conn->Query("CREATE TABLE IF NOT EXISTS sessions ("
                                "client_ip VARCHAR, "
                                "client_port VARCHAR, "
                                "server_ip VARCHAR, "
                                "server_port VARCHAR, "
                                "protocol VARCHAR, "
                                "frames VARCHAR, "
                                "timestamp VARCHAR)");
                                
    if (result->HasError()) {
        qDebug() << "DuckDB Error:" << QString::fromStdString(result->GetError());
    }
}

duckdb::Connection& DatabaseManager::getConnection() {
    return *m_conn;
}

void DatabaseManager::insertSession(const QString& clientIp, const QString& clientPort, const QString& serverIp, const QString& serverPort, const QString& protocol, const QString& frames, const QString& timestamp) {
    if (!m_conn) return;
    auto stmt = m_conn->Prepare("INSERT INTO sessions VALUES (?, ?, ?, ?, ?, ?, ?)");
    if (!stmt->HasError()) {
        auto result = stmt->Execute(clientIp.toStdString(), clientPort.toStdString(), serverIp.toStdString(), serverPort.toStdString(), protocol.toStdString(), frames.toStdString(), timestamp.toStdString());
        if (result->HasError()) {
            qDebug() << "DuckDB Insert Error:" << QString::fromStdString(result->GetError());
        }
    }
}
