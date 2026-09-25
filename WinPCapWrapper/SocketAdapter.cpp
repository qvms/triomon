#include "SocketAdapter.hpp"

namespace WinPCapWrapper {

SocketAdapter::SocketAdapter(const QNetworkInterface& nic, const QHostAddress& ip)
    : m_ip(ip), m_nic(nic)
{
    // Determine base packet type based on protocol
    if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
        m_basePacketType = SharedUtils::Pcap::PacketTypes::IPv4Packet;
    } else if (ip.protocol() == QAbstractSocket::IPv6Protocol) {
        m_basePacketType = SharedUtils::Pcap::PacketTypes::IPv6Packet;
    } else {
        m_basePacketType = SharedUtils::Pcap::PacketTypes::IPv4Packet;
    }
}

SharedUtils::Pcap::PacketTypes SocketAdapter::getBasePacketType() const {
    return m_basePacketType;
}

QHostAddress SocketAdapter::getIP() const {
    return m_ip;
}

QString SocketAdapter::toString() const {
    if (!m_nic.flags().testFlag(QNetworkInterface::IsUp)) {
        return QString("Socket: %1 (disconnected)").arg(m_nic.humanReadableName());
    } else {
        return QString("Socket: %1 (%2)").arg(m_nic.humanReadableName(), m_ip.toString());
    }
}

std::vector<std::shared_ptr<IAdapter>> SocketAdapter::getAdapters() {
    std::vector<std::shared_ptr<IAdapter>> adapters;
    
    QList<QNetworkInterface> nics = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& nic : nics) {
        QList<QNetworkAddressEntry> entries = nic.addressEntries();
        for (const QNetworkAddressEntry& entry : entries) {
            QHostAddress ip = entry.ip();
            if (!ip.isNull() && !ip.isLoopback() && ip.isGlobal()) {
                adapters.push_back(std::make_shared<SocketAdapter>(nic, ip));
            }
        }
    }
    
    return adapters;
}

} // namespace WinPCapWrapper
