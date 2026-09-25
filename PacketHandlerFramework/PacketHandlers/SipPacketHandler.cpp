#include "SipPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

SipPacketHandler::SipPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> SipPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::SipPacket>() };
}

ApplicationLayerProtocol SipPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::Sip;
}

void SipPacketHandler::ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::UdpPacket> udpPacket;
    std::shared_ptr<PacketParser::Packets::SipPacket> sipPacket;
    
    for (const auto& p : packetList) {
        if (auto udp = std::dynamic_pointer_cast<PacketParser::Packets::UdpPacket>(p)) {
            udpPacket = udp;
        } else if (auto sip = std::dynamic_pointer_cast<PacketParser::Packets::SipPacket>(p)) {
            sipPacket = sip;
        }
    }
    
    if (udpPacket && sipPacket) {
        QMap<QString, QString> tmpCol;
        if (!sipPacket->Method().isEmpty()) tmpCol.insert("Method", sipPacket->Method());
        if (!sipPacket->RequestURI().isEmpty()) tmpCol.insert("RequestURI", sipPacket->RequestURI());
        if (!sipPacket->From().isEmpty()) tmpCol.insert("From", sipPacket->From());
        if (!sipPacket->To().isEmpty()) tmpCol.insert("To", sipPacket->To());
        if (!sipPacket->CallID().isEmpty()) tmpCol.insert("Call-ID", sipPacket->CallID());
        if (!sipPacket->ContentType().isEmpty()) tmpCol.insert("Content-Type", sipPacket->ContentType());

        if (!sipPacket->From().isEmpty()) {
            sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SipFrom, sipPacket->From());
        }
        if (!sipPacket->To().isEmpty()) {
            destinationHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SipTo, sipPacket->To());
        }
        
        if (tmpCol.size() > 0) {
            MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                sipPacket->ParentFrame()->GetFrameNumber(),
                sourceHost, destinationHost, udpPacket->TransportProtocol(),
                udpPacket->SourcePort(), udpPacket->DestinationPort(),
                tmpCol, sipPacket->ParentFrame()->GetTimestamp(), "SIP packet"
            ));
        }
    }
}

int SipPacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket;
    std::shared_ptr<PacketParser::Packets::SipPacket> sipPacket;
    
    for (const auto& p : packetList) {
        if (auto tcp = std::dynamic_pointer_cast<PacketParser::Packets::TcpPacket>(p)) {
            tcpPacket = tcp;
        } else if (auto sip = std::dynamic_pointer_cast<PacketParser::Packets::SipPacket>(p)) {
            sipPacket = sip;
        }
    }
    
    if (tcpPacket && sipPacket) {
        QMap<QString, QString> tmpCol;
        if (!sipPacket->Method().isEmpty()) tmpCol.insert("Method", sipPacket->Method());
        if (!sipPacket->RequestURI().isEmpty()) tmpCol.insert("RequestURI", sipPacket->RequestURI());
        if (!sipPacket->From().isEmpty()) tmpCol.insert("From", sipPacket->From());
        if (!sipPacket->To().isEmpty()) tmpCol.insert("To", sipPacket->To());
        if (!sipPacket->CallID().isEmpty()) tmpCol.insert("Call-ID", sipPacket->CallID());
        if (!sipPacket->ContentType().isEmpty()) tmpCol.insert("Content-Type", sipPacket->ContentType());

        if (!sipPacket->From().isEmpty()) {
            tcpSession->GetSourceHost(transferIsClientToServer)->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SipFrom, sipPacket->From());
        }
        if (!sipPacket->To().isEmpty()) {
            tcpSession->GetDestinationHost(transferIsClientToServer)->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SipTo, sipPacket->To());
        }
        
        if (tmpCol.size() > 0) {
            MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                sipPacket->ParentFrame()->GetFrameNumber(),
                tcpSession->GetFlow()->FiveTuple(), transferIsClientToServer,
                tmpCol, sipPacket->ParentFrame()->GetTimestamp(), "SIP packet"
            ));
            return sipPacket->ParsedBytesCount();
        }
    }
    return 0;
}

void SipPacketHandler::Reset() {
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
