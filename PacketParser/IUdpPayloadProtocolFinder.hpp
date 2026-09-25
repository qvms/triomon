#pragma once
#include <QString>
#include <QHostAddress>
#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QHash>

namespace PacketParser {
class IUdpPayloadProtocolFinder {
public:
    virtual ~IUdpPayloadProtocolFinder() = default;

    virtual PacketParser::ApplicationLayerProtocol GetApplicationLayerProtocol(Packets::UdpPacket udpPacket) = 0;
    virtual void SetPayload(QHostAddress sourceIP, uint16_t sourcePort, QHostAddress destinationIP, uint16_t destinationPort, ApplicationLayerProtocol protocol) = 0;
};

} // namespace PacketParser
