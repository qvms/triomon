#include "IPHelper.hpp"
#include <QHostAddress>

namespace SharedUtils {

bool IPHelper::IsPrivateIP(const std::string& ipAddress) {
    QHostAddress address(QString::fromStdString(ipAddress));
    if (address.isNull()) return false;

    // IPv4 private ranges: 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        quint32 ipv4 = address.toIPv4Address();
        if ((ipv4 & 0xFF000000) == 0x0A000000) return true; // 10.x.x.x
        if ((ipv4 & 0xFFF00000) == 0xAC100000) return true; // 172.16.x.x - 172.31.x.x
        if ((ipv4 & 0xFFFF0000) == 0xC0A80000) return true; // 192.168.x.x
    }
    
    // IPv6 private/local ranges can be added as needed.
    return false;
}

bool IPHelper::IsValidIP(const std::string& ipAddress) {
    QHostAddress address(QString::fromStdString(ipAddress));
    return !address.isNull();
}

}
