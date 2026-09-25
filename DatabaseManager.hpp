#pragma once
#include <duckdb.hpp>
#include <QString>
#include <memory>
#include <mutex>

class DatabaseManager {
public:
    static DatabaseManager& instance();
    void init();
    duckdb::Connection& getConnection();
    
    // Test method
    void insertSession(const QString& clientIp, const QString& clientPort, const QString& serverIp, const QString& serverPort, const QString& protocol, const QString& frames, const QString& timestamp);

private:
    DatabaseManager() = default;
    ~DatabaseManager() = default;

    QString determineDatabasePath();

    std::unique_ptr<duckdb::DuckDB> m_db;
    std::unique_ptr<duckdb::Connection> m_conn;
};
