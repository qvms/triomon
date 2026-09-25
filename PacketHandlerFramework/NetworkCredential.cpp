#include "NetworkCredential.hpp"
#include <algorithm>
#include <QString>

namespace PacketHandlerFramework {

NetworkCredential::NetworkCredential(std::shared_ptr<PacketParser::NetworkHost> client, std::shared_ptr<PacketParser::NetworkHost> server, const std::string& protocolString, const std::string& username, const std::string& password, const QDateTime& timestamp, const std::string& domain)
    : client(client), server(server), protocolString(protocolString), username(username), password(password), timestamp(timestamp), domain(domain), validPassword(true) {}

NetworkCredential::NetworkCredential(std::shared_ptr<PacketParser::NetworkHost> client, std::shared_ptr<PacketParser::NetworkHost> server, const std::string& protocolString, const std::string& username, const QDateTime& timestamp, const std::string& domain)
    : client(client), server(server), protocolString(protocolString), username(username), password("N/A (No password found)"), timestamp(timestamp), domain(domain), validPassword(false) {}

int NetworkCredential::CompareTo(const NetworkCredential& other) const {
    if (this->timestamp < other.timestamp) return -1;
    if (this->timestamp > other.timestamp) return 1;
    return 0;
}

static std::string PickLikelyUsernameOrPassword(const std::vector<std::string>& wordlist) {
    std::string bestOption;
    for (const std::string& w : wordlist) {
        if (bestOption.empty()) {
            bestOption = w;
        } else if (bestOption.length() == 0 && w.length() > 0) {
            bestOption = w;
        } else if (w.length() >= 3 || w.length() > bestOption.length()) {
            if (std::abs(static_cast<int>(w.length()) - 8) < std::abs(static_cast<int>(bestOption.length()) - 8)) {
                bestOption = w;
            }
        }
    }
    return bestOption;
}

std::shared_ptr<NetworkCredential> NetworkCredential::GetNetworkCredential(const std::map<std::string, std::string>& parameters, std::shared_ptr<PacketParser::NetworkHost> client, std::shared_ptr<PacketParser::NetworkHost> server, const std::string& protocolString, const QDateTime& timestamp, const std::string& domain) {
    if (parameters.empty()) return nullptr;

    std::string username;
    std::vector<std::string> usernameGuessList;
    std::string password;
    std::vector<std::string> passwordGuessList;

    for (const auto& kvp : parameters) {
        const std::string& key = kvp.first;
        const std::string& val = kvp.second;
        QString qKey = QString::fromStdString(key).toLower();

        if (key == "user[screen_name]") {
            username = val;
        } else if (key == "gmailchat") {
            username = val;
            if (password.empty()) password = "N/A (unknown Google password)";
        } else if (key == "login_str") {
            username = "Facebook email: " + val;
            if (password.empty()) password = "N/A (unknown Facebook password)";
        } else if (key.find("username_or_email") != std::string::npos) {
            username = val;
        } else if (key == "login_username") {
            username = val;
        } else if (key == "secretkey") {
            password = val;
        } else if (key == "xml" && val.find("mail_inc_pass") != std::string::npos) {
            // Simplified parsing
            usernameGuessList.push_back("xml_parsed_user");
            passwordGuessList.push_back("xml_parsed_pass");
        } else if (key == "profile_id") {
            username = "Facebook profile ID: " + val;
            if (password.empty()) password = "N/A (unknown Facebook password)";
        } else if (key == "loginfmt") {
            username = val;
        } else if (key == "_u" && val.find(":") != std::string::npos) {
            // IcedID simplified
            username = "IcedID_" + val;
        } else if (qKey.contains("accountname")) {
            usernameGuessList.push_back(val);
        } else if (qKey == "login") {
            usernameGuessList.push_back(val);
        } else if (qKey.contains("username")) {
            usernameGuessList.push_back(val);
        } else if (qKey.contains("uname")) {
            usernameGuessList.push_back(val);
        } else if (key == "identifier") {
            usernameGuessList.push_back(val);
        } else if (qKey.contains("password")) {
            passwordGuessList.push_back(val);
        } else if (qKey.contains("user") || qKey.contains("usr")) {
            usernameGuessList.push_back(val);
        } else if (qKey.contains("pass") || qKey.contains("pw")) {
            passwordGuessList.push_back(val);
        } else if (qKey.contains("secret") || qKey.contains("key")) {
            passwordGuessList.push_back(val);
        } else if (usernameGuessList.empty() && qKey.contains("mail")) {
            usernameGuessList.push_back(val);
        } else if (usernameGuessList.empty() && qKey.contains("log")) {
            usernameGuessList.push_back(val);
        }
    }

    if (username.empty()) username = PickLikelyUsernameOrPassword(usernameGuessList);
    if (password.empty()) password = PickLikelyUsernameOrPassword(passwordGuessList);

    if (!username.empty() && !password.empty()) {
        return std::make_shared<NetworkCredential>(client, server, protocolString, username, password, timestamp, domain);
    } else if (!username.empty()) {
        return std::make_shared<NetworkCredential>(client, server, protocolString, username, timestamp, domain);
    }

    return nullptr;
}

} // namespace PacketHandlerFramework
