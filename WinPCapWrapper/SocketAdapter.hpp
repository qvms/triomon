#pragma once

#include "IAdapter.hpp"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QString>
#include <memory>
#include <vector>
#include "SharedUtils/Pcap/PacketReceivedEventArgs.hpp"

namespace WinPCapWrapper {

class SocketAdapter : public IAdapter {
private:
    QHostAddress m_ip;
    QNetworkInterface m_nic;
    SharedUtils::Pcap::PacketTypes m_basePacketType;

public:
    SocketAdapter(const QNetworkInterface& nic, const QHostAddress& ip);
    virtual ~SocketAdapter() = default;

    SharedUtils::Pcap::PacketTypes getBasePacketType() const;
    QHostAddress getIP() const;

    QString toString() const override;

    static std::vector<std::shared_ptr<IAdapter>> getAdapters();
};

} // namespace WinPCapWrapper
