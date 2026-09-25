#pragma once

#include <QByteArray>
#include <QDateTime>

namespace NetworkWrapper {

class PacketReceivedEventArgs {
public:
    enum class PacketTypes {
        Ethernet2Packet,
        IPv4Packet,
        IEEE_802_11Packet,
        IEEE_802_11RadiotapPacket
    };

private:
    QByteArray data;
    QDateTime timestamp;
    PacketTypes packetType;

public:
    PacketReceivedEventArgs(const QByteArray& data, const QDateTime& timestamp, PacketTypes packetType)
        : data(data), timestamp(timestamp), packetType(packetType) {}

    QByteArray Data() const { return data; }
    QDateTime Timestamp() const { return timestamp; }
    PacketTypes PacketType() const { return packetType; }
};

} // namespace NetworkWrapper
