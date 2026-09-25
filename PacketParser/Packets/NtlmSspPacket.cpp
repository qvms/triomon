#include "NtlmSspPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

NtlmSspPacket::SecurityBuffer::SecurityBuffer(const uint8_t* data, int& dataOffset) {
    length = Utils::ByteConverter::ToUInt16(data, dataOffset, true);
    dataOffset += 2;
    lengthAllocated = Utils::ByteConverter::ToUInt16(data, dataOffset, true);
    dataOffset += 2;
    offset = Utils::ByteConverter::ToUInt32(data, dataOffset, true);
    dataOffset += 4;
}

std::vector<uint8_t> NtlmSspPacket::SecurityBuffer::GetBufferData(const uint8_t* frameData, int packetStartIndex) const {
    std::vector<uint8_t> buffer;
    if (length > 0) {
        buffer.assign(frameData + packetStartIndex + offset, frameData + packetStartIndex + offset + length);
    }
    return buffer;
}

bool NtlmSspPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result) {
    result = nullptr;
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 8 > static_cast<int>(dataLen)) return false;

    int idx = packetStartIndex;
    QString sig = Utils::ByteConverter::ReadNullTerminatedString(data, dataLen, idx, false, false, 8);
    if (sig != "NTLMSSP") return false;

    try {
        result = new NtlmSspPacket(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } catch (...) {
        if (result) delete result;
        result = nullptr;
        return false;
    }
}

NtlmSspPacket::NtlmSspPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "NTLMSSP")
{
    int packetIndex = packetStartIndex;
    if (Utils::ByteConverter::ReadNullTerminatedString(parentFrame->Data(), parentFrame->DataLength(), packetIndex, false, false, 8) != "NTLMSSP") {
        throw std::runtime_error("Expected NTLMSSP signature string missing!");
    }
    
    messageType = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetIndex);
    packetIndex += 4;

    if (messageType == static_cast<uint32_t>(NtlmMessageTypes::Negotiate)) {
        // Negotiate parsing
    } else if (messageType == static_cast<uint32_t>(NtlmMessageTypes::Challenge)) {
        SecurityBuffer targetNameSecurityBuffer(parentFrame->Data(), packetIndex);
        
        uint8_t flags[4];
        for (int i=0; i<4; i++) {
            flags[i] = parentFrame->Data()[packetIndex + i];
        }
        bool dataIsUnicode = (flags[3] & 0x01) == 0x01;
        packetIndex += 4; // skip past flags

        if (targetNameSecurityBuffer.length > 0) {
            dataIsUnicode = (parentFrame->Data()[packetStartIndex + targetNameSecurityBuffer.offset + targetNameSecurityBuffer.length - 1] == 0x00);
        }

        ntlmChallenge = Utils::ByteConverter::ToHexString(parentFrame->Data(), 8, packetIndex);
        if (!parentFrame->QuickParse()) {
            addAttribute("NTLM Challenge", ntlmChallenge);
        }

        if (targetNameSecurityBuffer.length > 0) {
            packetIndex = packetStartIndex + targetNameSecurityBuffer.offset;
            if ((flags[1] & 0x03) == 0x02) {
                hostName = Utils::ByteConverter::ReadStringOriginal(parentFrame->Data(), packetIndex, targetNameSecurityBuffer.length, dataIsUnicode, true);
                if (!parentFrame->QuickParse()) {
                    addAttribute("Target Host Name", hostName);
                }
            } else {
                domainName = Utils::ByteConverter::ReadStringOriginal(parentFrame->Data(), packetIndex, targetNameSecurityBuffer.length, dataIsUnicode, true);
                if (!parentFrame->QuickParse()) {
                    addAttribute("Target Domain Name", domainName);
                }
            }
        }
    } else if (messageType == static_cast<uint32_t>(NtlmMessageTypes::Authentication)) {
        bool dataIsUnicode = false;
        SecurityBuffer lanManagerSecurityBuffer(parentFrame->Data(), packetIndex);
        SecurityBuffer ntLanManagerSecurityBuffer(parentFrame->Data(), packetIndex);
        SecurityBuffer domainNameSecurityBuffer(parentFrame->Data(), packetIndex);
        SecurityBuffer userNameSecurityBuffer(parentFrame->Data(), packetIndex);
        SecurityBuffer workstationNameSecurityBuffer(parentFrame->Data(), packetIndex);
        SecurityBuffer sessionKeySecurityBuffer(parentFrame->Data(), packetIndex);

        if (domainNameSecurityBuffer.length > 0) {
            dataIsUnicode = (parentFrame->Data()[packetStartIndex + domainNameSecurityBuffer.offset + domainNameSecurityBuffer.length - 1] == 0x00);
        } else if (userNameSecurityBuffer.length > 0) {
            dataIsUnicode = (parentFrame->Data()[packetStartIndex + userNameSecurityBuffer.offset + userNameSecurityBuffer.length - 1] == 0x00);
        } else if (workstationNameSecurityBuffer.length > 0) {
            dataIsUnicode = (parentFrame->Data()[packetStartIndex + workstationNameSecurityBuffer.offset + workstationNameSecurityBuffer.length - 1] == 0x00);
        }

        if (lanManagerSecurityBuffer.length > 0) {
            std::vector<uint8_t> bufferData = lanManagerSecurityBuffer.GetBufferData(parentFrame->Data(), packetStartIndex);
            lanManagerResponse = Utils::ByteConverter::ToHexString(bufferData.data(), bufferData.size(), 0);
            if (!parentFrame->QuickParse()) {
                addAttribute("LAN Manager Response", lanManagerResponse);
            }
        }
        if (ntLanManagerSecurityBuffer.length > 0) {
            std::vector<uint8_t> bufferData = ntLanManagerSecurityBuffer.GetBufferData(parentFrame->Data(), packetStartIndex);
            ntlmResponse = Utils::ByteConverter::ToHexString(bufferData.data(), bufferData.size(), 0);
            if (!parentFrame->QuickParse()) {
                addAttribute("NTLM Response", ntlmResponse);
            }
        }
        if (domainNameSecurityBuffer.length > 0) {
            packetIndex = packetStartIndex + domainNameSecurityBuffer.offset;
            domainName = Utils::ByteConverter::ReadStringOriginal(parentFrame->Data(), packetIndex, domainNameSecurityBuffer.length, dataIsUnicode, true);
            if (!parentFrame->QuickParse()) {
                addAttribute("Domain Name", domainName);
            }
        }
        if (userNameSecurityBuffer.length > 0) {
            packetIndex = packetStartIndex + userNameSecurityBuffer.offset;
            userName = Utils::ByteConverter::ReadStringOriginal(parentFrame->Data(), packetIndex, userNameSecurityBuffer.length, dataIsUnicode, true);
            if (!parentFrame->QuickParse()) {
                addAttribute("User Name", userName);
            }
        }
        if (workstationNameSecurityBuffer.length > 0) {
            packetIndex = packetStartIndex + workstationNameSecurityBuffer.offset;
            hostName = Utils::ByteConverter::ReadStringOriginal(parentFrame->Data(), packetIndex, workstationNameSecurityBuffer.length, dataIsUnicode, true);
            if (!parentFrame->QuickParse()) {
                addAttribute("Host Name", hostName);
            }
        }
    }
}

std::vector<AbstractPacket*> NtlmSspPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
