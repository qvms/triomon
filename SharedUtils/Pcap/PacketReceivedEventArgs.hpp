#ifndef PACKETRECEIVEDEVENTARGS_HPP
#define PACKETRECEIVEDEVENTARGS_HPP

#include <QByteArray>
#include <QDateTime>
#include <QObject>
#include "../../PacketParser/PcapFrameDataLinkTypeEnum.hpp"

class PacketReceivedEventArgs {
public:
    enum class PacketTypes {
        NullLoopback,
        Ethernet2Packet,
        IPv4Packet,
        IPv6Packet,
        IEEE_802_11Packet,
        IEEE_802_11RadiotapPacket,
        CiscoHDLC,
        LinuxCookedCapture,
        LinuxCookedCapture2,
        PrismCaptureHeader,
        TZSP
    };

    static bool tryGetPacketType(quint32 dataLinkType, PacketTypes& basePacketType);
    static bool tryGetPacketType(PcapFrameDataLinkTypeEnum dlt, PacketTypes& basePacketType);

    PacketReceivedEventArgs(const QByteArray& data, const QDateTime& timestamp, PacketTypes packetType);

    QDateTime getTimestamp() const { return timestamp; }
    QByteArray getData() const { return data; }
    PacketTypes getPacketType() const { return packetType; }

private:
    QDateTime timestamp;
    QByteArray data;
    PacketTypes packetType;
};

#endif // PACKETRECEIVEDEVENTARGS_HPP
