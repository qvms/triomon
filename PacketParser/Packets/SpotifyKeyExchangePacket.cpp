#include "SpotifyKeyExchangePacket.hpp"
#include "../Utils.hpp"
#include <cstring>

namespace PacketParser {
namespace Packets {

const uint16_t CONTENT_END_USHORT = 0x0140;

SpotifyKeyExchangePacket::SpotifyKeyExchangePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool isFromClient)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Spotify Key Exchange")
    , m_version(0), m_keyExchangePacketLength(0), m_clientOS(0), m_clientID(0), m_clientRevision(0), m_fromClient(isFromClient)
{
    if (isFromClient) {
        m_version = parentFrame->Data()[packetStartIndex];
        if (m_version == 0x02) {
            m_keyExchangePacketLength = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2);
            if (packetStartIndex + m_keyExchangePacketLength - 1 < packetEndIndex) {
                setPacketEndIndex(packetStartIndex + m_keyExchangePacketLength - 1);
            }
            m_clientOS = parentFrame->Data()[packetStartIndex + 4];
            if (!ParentFrame()->QuickParse()) {
                addAttribute("Client OS", ClientOperatingSystem());
            }
            m_clientID = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 5);
            if (!ParentFrame()->QuickParse()) {
                addAttribute("Client ID", "0x" + QString::number(m_clientID, 16).rightJustified(8, '0').toUpper());
            }
            m_clientRevision = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 9);
            if (!ParentFrame()->QuickParse()) {
                addAttribute("Client Revision", QString::number(m_clientRevision));
            }
            
            m_random.resize(16);
            std::memcpy(m_random.data(), parentFrame->Data() + packetStartIndex + 13, 16);
            
            m_publicKey.resize(96);
            std::memcpy(m_publicKey.data(), parentFrame->Data() + packetStartIndex + 29, 96);
            
            m_blob.resize(128);
            std::memcpy(m_blob.data(), parentFrame->Data() + packetStartIndex + 125, 128);
            
            uint8_t usernameLength = parentFrame->Data()[packetStartIndex + 253];
            m_username = Utils::ByteConverter::ReadString(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex + 254, usernameLength, false);
            
            if (!ParentFrame()->QuickParse()) {
                addAttribute("Client Username", m_username);
            }
            
            if (Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 254 + usernameLength) != CONTENT_END_USHORT) {
                if (!ParentFrame()->QuickParse()) const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "Not a valid SpotifyKeyExchangePacket"));
            }
        } else if (m_version == 0x03) {
            m_keyExchangePacketLength = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2);
            if (packetStartIndex + m_keyExchangePacketLength - 1 < packetEndIndex) {
                setPacketEndIndex(packetStartIndex + m_keyExchangePacketLength - 1);
            }
            m_clientRevision = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 12);
            if (!ParentFrame()->QuickParse()) {
                addAttribute("Client Revision", QString::number(m_clientRevision));
            }
            m_clientID = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 24);
            if (!ParentFrame()->QuickParse()) {
                addAttribute("Client ID", "0x" + QString::number(m_clientID, 16).rightJustified(8, '0').toUpper());
            }
            
            m_publicKey.resize(96);
            std::memcpy(m_publicKey.data(), parentFrame->Data() + packetStartIndex + 48, 96);
            
            uint8_t randomDataLength = parentFrame->Data()[packetStartIndex + 272];
            uint8_t usernameLength = parentFrame->Data()[packetStartIndex + 273];
            
            m_username = Utils::ByteConverter::ReadString(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex + 276 + randomDataLength, usernameLength, false);
            if (!ParentFrame()->QuickParse()) {
                addAttribute("Client Username", m_username);
            }
        }
    } else {
        int expectedVersionNumber = -1;
        
        if (packetStartIndex + 380 + parentFrame->Data()[packetStartIndex + 17] <= packetEndIndex && 
            packetEndIndex - packetStartIndex + 1 == 380 + parentFrame->Data()[packetStartIndex + 17] + parentFrame->Data()[packetStartIndex + 380 + parentFrame->Data()[packetStartIndex + 17]]) {
            expectedVersionNumber = 2;
        } else if (packetStartIndex + 0x182 + 1 <= packetEndIndex && 
                   packetEndIndex - packetStartIndex + 1 == 0x184 + parentFrame->Data()[packetStartIndex + 0x17a] + parentFrame->Data()[packetStartIndex + 0x17b] + 
                   Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 0x17c) + 
                   Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 0x17e) + 
                   Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 0x180) + 
                   Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 0x182)) {
            expectedVersionNumber = 3;
        }
        
        if (expectedVersionNumber == 2) {
            m_random.resize(16);
            std::memcpy(m_random.data(), parentFrame->Data() + packetStartIndex, 16);
            
            uint8_t usernameLength = parentFrame->Data()[packetStartIndex + 17];
            m_username = Utils::ByteConverter::ReadString(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex + 18, usernameLength, false);
            if (!ParentFrame()->QuickParse()) {
                addAttribute("Client Username", m_username);
            }
            
            m_publicKey.resize(96);
            std::memcpy(m_publicKey.data(), parentFrame->Data() + packetStartIndex + 18 + usernameLength, 96);
            
            m_salt.resize(10);
            std::memcpy(m_salt.data(), parentFrame->Data() + packetStartIndex + 370 + usernameLength, 10);
        } else if (expectedVersionNumber == 3) {
            m_random.resize(16);
            std::memcpy(m_random.data(), parentFrame->Data() + packetStartIndex, 16);
            
            m_publicKey.resize(96);
            std::memcpy(m_publicKey.data(), parentFrame->Data() + packetStartIndex + 16, 96);
            
            uint8_t paddingLength = parentFrame->Data()[packetStartIndex + 0x17a];
            uint8_t usernameLength = parentFrame->Data()[packetStartIndex + 0x17b];
            
            m_username = Utils::ByteConverter::ReadString(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex + 0x184 + paddingLength, usernameLength, false);
            if (!ParentFrame()->QuickParse()) {
                addAttribute("Client Username", m_username);
            }
        }
    }
}

QString SpotifyKeyExchangePacket::ClientOperatingSystem() const {
    switch (m_clientOS) {
        case 0: return "Windows";
        case 1: return "Mac OS X";
        case 2: return "Linux";
        case 3: return "Symbian";
        case 5: return "iPhone";
        default: return QString::number(m_clientOS);
    }
}

bool SpotifyKeyExchangePacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool fromClient, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;
    
    if (fromClient) {
        if (packetEndIndex - packetStartIndex > 276) {
            uint8_t version = parentFrame->Data()[packetStartIndex];
            uint16_t length = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2);
            if (packetStartIndex + length - 1 <= packetEndIndex || packetStartIndex + length - 1 > packetEndIndex) {
                if (version == 0x02 && parentFrame->Data()[packetStartIndex + 1] == 0x04) {
                    try {
                        *result = new SpotifyKeyExchangePacket(parentFrame, packetStartIndex, packetEndIndex, fromClient);
                        return true;
                    } catch (...) { return false; }
                } else if (version == 0x03 && parentFrame->Data()[packetStartIndex + 1] == 0x04) {
                    try {
                        *result = new SpotifyKeyExchangePacket(parentFrame, packetStartIndex, packetEndIndex, fromClient);
                        return true;
                    } catch (...) { return false; }
                }
            }
        }
    } else {
        if (packetEndIndex - packetStartIndex > 380) {
            int expectedVersionNumber = -1;
            
            if (packetStartIndex + 380 + parentFrame->Data()[packetStartIndex + 17] <= packetEndIndex && 
                packetEndIndex - packetStartIndex + 1 == 380 + parentFrame->Data()[packetStartIndex + 17] + parentFrame->Data()[packetStartIndex + 380 + parentFrame->Data()[packetStartIndex + 17]]) {
                expectedVersionNumber = 2;
            } else if (packetStartIndex + 0x182 + 1 <= packetEndIndex && 
                       packetEndIndex - packetStartIndex + 1 == 0x184 + parentFrame->Data()[packetStartIndex + 0x17a] + parentFrame->Data()[packetStartIndex + 0x17b] + 
                       Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 0x17c) + 
                       Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 0x17e) + 
                       Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 0x180) + 
                       Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 0x182)) {
                expectedVersionNumber = 3;
            }
            
            if (expectedVersionNumber > 0) {
                try {
                    *result = new SpotifyKeyExchangePacket(parentFrame, packetStartIndex, packetEndIndex, fromClient);
                    return true;
                } catch (...) { return false; }
            }
        }
    }
    
    return false;
}

std::vector<AbstractPacket*> SpotifyKeyExchangePacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
