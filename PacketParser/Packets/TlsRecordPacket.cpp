#include "TlsRecordPacket.hpp"
#include "RawPacket.hpp"
#include "../Utils/ByteConverter.hpp"
#include <algorithm>
#include <stdexcept>
#include <QString>

namespace PacketParser {
namespace Packets {

namespace {
    QString contentTypeToString(TlsRecordPacket::ContentTypes type) {
        switch (type) {
            case TlsRecordPacket::ContentTypes::ChangeCipherSpec: return "ChangeCipherSpec";
            case TlsRecordPacket::ContentTypes::Alert: return "Alert";
            case TlsRecordPacket::ContentTypes::Handshake: return "Handshake";
            case TlsRecordPacket::ContentTypes::Application: return "Application";
            default: return QString::number(static_cast<int>(type));
        }
    }

    QString messageTypeToString(TlsRecordPacket::HandshakePacket::MessageTypes type) {
        switch (type) {
            case TlsRecordPacket::HandshakePacket::MessageTypes::HelloRequest: return "HelloRequest";
            case TlsRecordPacket::HandshakePacket::MessageTypes::ClientHello: return "ClientHello";
            case TlsRecordPacket::HandshakePacket::MessageTypes::ServerHello: return "ServerHello";
            case TlsRecordPacket::HandshakePacket::MessageTypes::Certificate: return "Certificate";
            case TlsRecordPacket::HandshakePacket::MessageTypes::ServerKeyExchange: return "ServerKeyExchange";
            case TlsRecordPacket::HandshakePacket::MessageTypes::CertificateRequest: return "CertificateRequest";
            case TlsRecordPacket::HandshakePacket::MessageTypes::ServerHelloDone: return "ServerHelloDone";
            case TlsRecordPacket::HandshakePacket::MessageTypes::CertificateVerify: return "CertificateVerify";
            case TlsRecordPacket::HandshakePacket::MessageTypes::ClientKeyExchange: return "ClientKeyExchange";
            case TlsRecordPacket::HandshakePacket::MessageTypes::Finished: return "Finished";
            default: return QString::number(static_cast<int>(type));
        }
    }

    uint32_t readUInt24(const uint8_t* data, int offset) {
        return (static_cast<uint32_t>(data[offset]) << 16) |
               (static_cast<uint32_t>(data[offset + 1]) << 8) |
               static_cast<uint32_t>(data[offset + 2]);
    }
}

TlsRecordPacket::TlsRecordPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "TLS Record") {
    
    if (static_cast<size_t>(packetStartIndex + 4) >= parentFrame->DataLength()) {
        throw std::out_of_range("Packet is too small to be a TLS Record.");
    }

    const uint8_t* data = parentFrame->Data();
    this->contentType = static_cast<ContentTypes>(data[packetStartIndex]);
    this->versionMajor = data[packetStartIndex + 1];
    this->versionMinor = data[packetStartIndex + 2];
    this->length = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 3, false);
    
    setPacketEndIndex(std::min(packetStartIndex + 5 + length - 1, PacketEndIndex()));

    addAttribute("Content Type", contentTypeToString(this->contentType));
    addAttribute("TLS Version major", QString::number(this->versionMajor));
    addAttribute("TLS Version minor", QString::number(this->versionMinor));
}

std::vector<AbstractPacket*> TlsRecordPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) {
        subPackets.push_back(this);
    }
    
    if (this->contentType == ContentTypes::Handshake) {
        int subPacketStartIndex = PacketStartIndex() + 5;
        while (subPacketStartIndex < PacketEndIndex()) {
            AbstractPacket* packet = nullptr;
            try {
                packet = new HandshakePacket(ParentFrame(), subPacketStartIndex, PacketEndIndex());
            } catch (...) {
                packet = new RawPacket(ParentFrame(), subPacketStartIndex, PacketEndIndex());
            }

            if (packet) {
                subPacketStartIndex = packet->PacketEndIndex() + 1;
                subPackets.push_back(packet);
            } else {
                break;
            }
        }
    }
    
    return subPackets;
}

TlsRecordPacket::HandshakePacket::HandshakePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "TLS Handshake Protocol") {
    
    if (static_cast<size_t>(packetStartIndex + 3) >= parentFrame->DataLength()) {
        throw std::out_of_range("Packet is too small to be a TLS Handshake Packet.");
    }

    const uint8_t* data = parentFrame->Data();
    this->messageType = static_cast<MessageTypes>(data[packetStartIndex]);
    addAttribute("Message Type", messageTypeToString(this->messageType));
    
    this->messageLength = readUInt24(data, packetStartIndex + 1);
    setPacketEndIndex(packetStartIndex + 4 + this->messageLength - 1);

    if (this->messageType == MessageTypes::Certificate) {
        if (static_cast<size_t>(packetStartIndex + 6) >= parentFrame->DataLength()) {
            throw std::out_of_range("Not enough data for certificates length");
        }
        uint32_t certificatesLength = readUInt24(data, packetStartIndex + 4);
        int certificateIndexBase = packetStartIndex + 7;
        uint32_t certificateIndexOffset = 0;
        
        while (certificateIndexOffset < certificatesLength) {
            if (static_cast<size_t>(certificateIndexBase + certificateIndexOffset + 2) >= parentFrame->DataLength()) {
                throw std::out_of_range("Not enough data for certificate length");
            }
            uint32_t certificateLength = readUInt24(data, certificateIndexBase + certificateIndexOffset);
            certificateIndexOffset += 3;
            
            if (static_cast<size_t>(certificateIndexBase + certificateIndexOffset + certificateLength) > parentFrame->DataLength()) {
                throw std::out_of_range("Not enough data for certificate data");
            }
            
            std::vector<uint8_t> certificate(
                data + certificateIndexBase + certificateIndexOffset,
                data + certificateIndexBase + certificateIndexOffset + certificateLength
            );
            
            this->certificateList.push_back(std::move(certificate));
            certificateIndexOffset += certificateLength;
        }
    }
}

std::vector<AbstractPacket*> TlsRecordPacket::HandshakePacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) {
        subPackets.push_back(this);
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
