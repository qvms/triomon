#pragma once

#include <QString>
#include <QDateTime>

namespace PacketHandlerFramework {

class NetworkServiceMetadata {
public:
    enum class ServiceType { Unknown, RDP, DNS, SSDP, SSDP_Browser };

private:
    QString host;
    int port;
    ServiceType serviceType;
    QString extraInfo;
    QDateTime timestamp;
    QByteArray certificateThumbprint;

public:
    NetworkServiceMetadata(const QString& host, int port, ServiceType serviceType, const QString& extraInfo, const QDateTime& timestamp)
        : host(host), port(port), serviceType(serviceType), extraInfo(extraInfo), timestamp(timestamp) {}

    QString Host() const { return host; }
    int Port() const { return port; }
    ServiceType GetServiceType() const { return serviceType; }
    QString ExtraInfo() const { return extraInfo; }
    QDateTime Timestamp() const { return timestamp; }
    QByteArray CertificateThumbprint() const { return certificateThumbprint; }
    void setCertificateThumbprint(const QByteArray& thumbprint) { certificateThumbprint = thumbprint; }
};

} // namespace PacketHandlerFramework
