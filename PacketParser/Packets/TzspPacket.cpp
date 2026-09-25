#include "TzspPacket.hpp"
#include "Ethernet2Packet.hpp"
#include "PointToPointPacket.hpp"
#include "IPv4Packet.hpp"
#include "IEEE_802_11Packet.hpp"
#include "IEEE_802_11RadiotapPacket.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

bool TzspPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& tzspPacket) {
    tzspPacket = nullptr;
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 3 >= static_cast<int>(dataLen)) return false;

    if (data[packetStartIndex] != 1) return false;

    uint16_t encap = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);
    // Rough check on valid encapsulations
    if (encap != static_cast<uint16_t>(TzspEncapsulation::Ethernet) &&
        encap != static_cast<uint16_t>(TzspEncapsulation::PPP) &&
        encap != static_cast<uint16_t>(TzspEncapsulation::RAW) &&
        encap != static_cast<uint16_t>(TzspEncapsulation::IEEE_802_11) &&
        encap != static_cast<uint16_t>(TzspEncapsulation::IEEE_802_11_PRISM) &&
        encap != static_cast<uint16_t>(TzspEncapsulation::IEEE_802_11_RADIOTAP) &&
        encap != static_cast<uint16_t>(TzspEncapsulation::IEEE_802_11_AVS)) {
        return false;
    }

    try {
        tzspPacket = new TzspPacket(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } catch (...) {
        return false;
    }
}

TzspPacket::TzspPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "TZSP")
    , m_version(0)
    , m_type(0)
    , m_encapsulation(0)
    , encapsulatedPacketStartIndex(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 3 >= static_cast<int>(dataLen)) {
        throw std::runtime_error("truncated");
    }

    m_version = data[packetStartIndex];
    m_type = data[packetStartIndex + 1];
    m_encapsulation = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2);

    bool foundEndTag = false;
    for (int i = packetStartIndex + 4; i <= packetEndIndex && i < static_cast<int>(dataLen); i++) {
        uint8_t tag = data[i];
        if (tag == static_cast<uint8_t>(TzspTag::End)) {
            foundEndTag = true;
            encapsulatedPacketStartIndex = i + 1;
            if (encapsulatedPacketStartIndex > packetEndIndex) {
                // throw new Exception("Malformed TZSP packet");
            }
            break;
        } else if (tag == static_cast<uint8_t>(TzspTag::Padding)) {
            continue;
        } else {
            if (i + 1 < static_cast<int>(dataLen)) {
                uint8_t length = data[i + 1];
                i += length + 1; // +1 because the loop adds 1
            } else {
                break;
            }
        }
    }
}

std::vector<AbstractPacket*> TzspPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (encapsulatedPacketStartIndex > 0 && encapsulatedPacketStartIndex <= PacketEndIndex()) {
        AbstractPacket* encapsulatedPacket = nullptr;

        if (m_encapsulation == static_cast<uint16_t>(TzspEncapsulation::Ethernet)) {
            encapsulatedPacket = new Ethernet2Packet(ParentFrame(), encapsulatedPacketStartIndex, PacketEndIndex());
        } else if (m_encapsulation == static_cast<uint16_t>(TzspEncapsulation::PPP)) {
            encapsulatedPacket = new PointToPointPacket(ParentFrame(), encapsulatedPacketStartIndex, PacketEndIndex());
        } else if (m_encapsulation == static_cast<uint16_t>(TzspEncapsulation::RAW)) {
            if (!IPv4Packet::TryParse(ParentFrame(), encapsulatedPacketStartIndex, PacketEndIndex(), encapsulatedPacket)) {
                encapsulatedPacket = nullptr;
            }
        } else if (m_encapsulation == static_cast<uint16_t>(TzspEncapsulation::IEEE_802_11)) {
            encapsulatedPacket = new IEEE_802_11Packet(ParentFrame(), encapsulatedPacketStartIndex, PacketEndIndex());
        } else if (m_encapsulation == static_cast<uint16_t>(TzspEncapsulation::IEEE_802_11_RADIOTAP)) {
            encapsulatedPacket = new IEEE_802_11RadiotapPacket(ParentFrame(), encapsulatedPacketStartIndex, PacketEndIndex());
        }

        if (!encapsulatedPacket) {
            encapsulatedPacket = new RawPacket(ParentFrame(), encapsulatedPacketStartIndex, PacketEndIndex());
        }

        subPackets.push_back(encapsulatedPacket);
        std::vector<AbstractPacket*> childSubPackets = encapsulatedPacket->GetSubPackets(false);
        subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
    }

    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
