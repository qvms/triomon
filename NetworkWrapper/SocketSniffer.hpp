#pragma once

#include "ISniffer.hpp"
#include "PacketReceivedEventArgs.hpp"
#include <QHostAddress>
#include <QUdpSocket>
#include <functional>
#include <QObject>

namespace NetworkWrapper {

class SocketAdapter {
public:
    QHostAddress IP;
};

class SocketSniffer : public QObject, public ISniffer {
    Q_OBJECT
private:
    QUdpSocket* socket;
    bool snifferActive;

public:
    using PacketReceivedHandler = std::function<void(SocketSniffer*, const PacketReceivedEventArgs&)>;
    static PacketReceivedHandler PacketReceived;

    SocketSniffer(const SocketAdapter& adapter, QObject* parent = nullptr);
    virtual ~SocketSniffer();

    PacketReceivedEventArgs::PacketTypes GetBasePacketType() const override { return PacketReceivedEventArgs::PacketTypes::IPv4Packet; }
    void StartSniffing() override;
    void StopSniffing() override;

private slots:
    void ReceivePacketListener();
};

} // namespace NetworkWrapper
