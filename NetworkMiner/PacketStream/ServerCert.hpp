#pragma once

#include <QSslCertificate>
#include <QSslKey>

namespace NetworkMiner {
namespace PacketStream {

class ServerCert {
public:
    static ServerCert* instance();

    QSslCertificate certificate() const;
    QSslKey privateKey() const;

private:
    ServerCert();
    ~ServerCert() = default;

    // Delete copy constructor and assignment operator
    ServerCert(const ServerCert&) = delete;
    ServerCert& operator=(const ServerCert&) = delete;

    QSslCertificate m_certificate;
    QSslKey m_privateKey;
};

} // namespace PacketStream
} // namespace NetworkMiner
