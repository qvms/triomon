#include "SyslogPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

SyslogPacketHandler::SyslogPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> SyslogPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::SyslogPacket>() };
}

void SyslogPacketHandler::ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::SyslogPacket> syslogPacket;
    std::shared_ptr<PacketParser::Packets::UdpPacket> udpPacket;

    for (const auto& p : packetList) {
        if (auto syslog = std::dynamic_pointer_cast<PacketParser::Packets::SyslogPacket>(p)) {
            syslogPacket = syslog;
        } else if (auto udp = std::dynamic_pointer_cast<PacketParser::Packets::UdpPacket>(p)) {
            udpPacket = udp;
        }

        if (syslogPacket && udpPacket && !syslogPacket->SyslogMessage().isEmpty()) {
            QMap<QString, QString> tmpCol;
            tmpCol.insert("Syslog Message", syslogPacket->SyslogMessage());

            MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                syslogPacket->ParentFrame()->GetFrameNumber(),
                sourceHost, destinationHost, udpPacket->TransportProtocol(),
                udpPacket->SourcePort(), udpPacket->DestinationPort(),
                tmpCol, syslogPacket->ParentFrame()->GetTimestamp(), "Syslog Message"
            ));
        }
    }
}

void SyslogPacketHandler::Reset() {
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
