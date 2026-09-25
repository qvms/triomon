#pragma once

#include <string>
#include <QDateTime>
#include <memory>
#include "PacketParser/NetworkHost.hpp"
#include <map>

namespace PacketHandlerFramework {

class NetworkCredential {
private:
    std::shared_ptr<PacketParser::NetworkHost> client;
    std::shared_ptr<PacketParser::NetworkHost> server;
    std::string protocolString;
    std::string username;
    std::string password;
    QDateTime timestamp;
    std::string domain;
    bool validPassword;

public:
    NetworkCredential(std::shared_ptr<PacketParser::NetworkHost> client, std::shared_ptr<PacketParser::NetworkHost> server, const std::string& protocolString, const std::string& username, const std::string& password, const QDateTime& timestamp, const std::string& domain = "");
    NetworkCredential(std::shared_ptr<PacketParser::NetworkHost> client, std::shared_ptr<PacketParser::NetworkHost> server, const std::string& protocolString, const std::string& username, const QDateTime& timestamp, const std::string& domain = "");

    std::shared_ptr<PacketParser::NetworkHost> Client() const { return client; }
    std::shared_ptr<PacketParser::NetworkHost> Server() const { return server; }
    std::string ProtocolString() const { return protocolString; }
    std::string Username() const { return username; }
    std::string Password() const { return password; }
    QDateTime Timestamp() const { return timestamp; }
    std::string Domain() const { return domain; }
    bool ValidPassword() const { return validPassword; }

    int CompareTo(const NetworkCredential& other) const;

    static std::shared_ptr<NetworkCredential> GetNetworkCredential(const std::map<std::string, std::string>& parameters, std::shared_ptr<PacketParser::NetworkHost> client, std::shared_ptr<PacketParser::NetworkHost> server, const std::string& protocolString, const QDateTime& timestamp, const std::string& domain = "");
};

} // namespace PacketHandlerFramework
