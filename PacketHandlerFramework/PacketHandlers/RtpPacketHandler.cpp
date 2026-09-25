#include "RtpPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

RtpPacketHandler::RtpPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> RtpPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::RtpPacket>() };
}

void RtpPacketHandler::ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::UdpPacket> udpPacket;
    
    for (const auto& p : packetList) {
        if (auto udp = std::dynamic_pointer_cast<PacketParser::Packets::UdpPacket>(p)) {
            udpPacket = udp;
        } else if (udpPacket) {
            if (auto rtpPacket = std::dynamic_pointer_cast<PacketParser::Packets::RtpPacket>(p)) {
                if (rtpPacket->PayloadType() == static_cast<quint8>(RtpPayloadType::G711_PCM_U) ||
                    rtpPacket->PayloadType() == static_cast<quint8>(RtpPayloadType::G711_PCM_A) ||
                    rtpPacket->PayloadType() == static_cast<quint8>(RtpPayloadType::G722) ||
                    rtpPacket->PayloadType() == static_cast<quint8>(RtpPayloadType::G729)) {
                    
                    RtpPayloadType payloadType = static_cast<RtpPayloadType>(rtpPacket->PayloadType());
                    PacketParser::FiveTuple fiveTuple(sourceHost, udpPacket->SourcePort(), destinationHost, udpPacket->DestinationPort(), PacketParser::FiveTuple::TransportProtocol::UDP);
                    
                    std::shared_ptr<NetworkMiner::AudioStream> audioStream;
                    StreamKey key = std::make_tuple(sourceHost->IPAddress().toIPv4Address(), udpPacket->SourcePort(), destinationHost->IPAddress().toIPv4Address(), udpPacket->DestinationPort(), static_cast<quint8>(payloadType));
                    
                    if (audioStreams.contains(key)) {
                        audioStream = audioStreams[key];
                    } else {
                        // TODO: Map RtpPayloadType to NetworkMiner::AudioStream::RtpPayloadType once ported
                        // audioStream = std::make_shared<NetworkMiner::AudioStream>(sourceHost, destinationHost, payloadType, MainPacketHandler()->FileStreamAssemblerList(), fiveTuple, rtpPacket->ParentFrame()->GetFrameNumber());
                        // audioStreams.insert(key, audioStream);
                        // MainPacketHandler()->OnAudioDetected(audioStream);
                    }
                    
                    if (audioStream) {
                        QByteArray samples = rtpPacket->ParentFrame()->GetData().mid(rtpPacket->PacketStartIndex() + PacketParser::Packets::RtpPacket::HEADER_LENGTH, rtpPacket->PacketLength() - PacketParser::Packets::RtpPacket::HEADER_LENGTH);
                        // audioStream->AddSamples(samples, rtpPacket->SampleTick(), rtpPacket->ParentFrame()->GetTimestamp(), rtpPacket->SyncSourceID());
                    }
                }
            }
        }
    }
}

void RtpPacketHandler::Reset() {
    audioStreams.clear();
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
