#include "PacketReceivedEventArgs.hpp"

bool PacketReceivedEventArgs::tryGetPacketType(quint32 dataLinkType, PacketTypes& basePacketType) {
    return tryGetPacketType(static_cast<PcapFrameDataLinkTypeEnum>(dataLinkType), basePacketType);
}

bool PacketReceivedEventArgs::tryGetPacketType(PcapFrameDataLinkTypeEnum dlt, PacketTypes& basePacketType) {
    if (dlt == PcapFrameDataLinkTypeEnum::WTAP_ENCAP_IEEE_802_11) {
        basePacketType = PacketTypes::IEEE_802_11Packet;
    } else if (dlt == PcapFrameDataLinkTypeEnum::WTAP_ENCAP_ETHERNET) {
        basePacketType = PacketTypes::Ethernet2Packet;
    } else if (dlt == PcapFrameDataLinkTypeEnum::WTAP_ENCAP_IEEE_802_11_WLAN_RADIOTAP) {
        basePacketType = PacketTypes::IEEE_802_11RadiotapPacket;
    } else if (dlt == PcapFrameDataLinkTypeEnum::WTAP_ENCAP_RAW_IP || 
               dlt == PcapFrameDataLinkTypeEnum::WTAP_ENCAP_RAW_IP_2 || 
               dlt == PcapFrameDataLinkTypeEnum::WTAP_ENCAP_RAW_IP_3) {
        basePacketType = PacketTypes::IPv4Packet;
    } else if (dlt == PcapFrameDataLinkTypeEnum::WTAP_ENCAP_CHDLC) {
        basePacketType = PacketTypes::CiscoHDLC;
    } else if (dlt == PcapFrameDataLinkTypeEnum::WTAP_ENCAP_SLL) {
        basePacketType = PacketTypes::LinuxCookedCapture;
    } else {
        basePacketType = PacketTypes::NullLoopback;
        return false;
    }
    return true;
}

PacketReceivedEventArgs::PacketReceivedEventArgs(const QByteArray& data, const QDateTime& timestamp, PacketTypes packetType)
    : timestamp(timestamp.toUTC()), data(data), packetType(packetType)
{
}
