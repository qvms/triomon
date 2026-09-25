#include "CifsBrowserPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

CifsBrowserPacket::CifsBrowserPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "MS-BRWS (CIFS Browser Protocol)")
    , uptimeSeconds(0)
    , hasUptime(false)
{
    command = parentFrame->Data()[packetStartIndex];
    
    if (command == static_cast<uint8_t>(BrowserMessageOpCode::DomainAnnouncement)) {
        int index = packetStartIndex + 6;
        domainOrWorkgroup = Utils::ByteConverter::ReadNullTerminatedString(parentFrame->Data(), parentFrame->DataLength(), index, false, false, 16);
        osVersion = {parentFrame->Data()[packetStartIndex + 32], parentFrame->Data()[packetStartIndex + 33]}; // original logic had OSVersion commented out but we'll try our best
        index = packetStartIndex + 32;
        if (index < packetEndIndex) {
            hostname = Utils::ByteConverter::ReadNullTerminatedString(parentFrame->Data(), parentFrame->DataLength(), index, false, false, 43);
        }
    } else if (command == static_cast<uint8_t>(BrowserMessageOpCode::LocalMasterAnnouncement) || command == static_cast<uint8_t>(BrowserMessageOpCode::HostAnnouncement)) {
        int index = packetStartIndex + 6;
        hostname = Utils::ByteConverter::ReadNullTerminatedString(parentFrame->Data(), parentFrame->DataLength(), index, false, false, 16);
        osVersion = {parentFrame->Data()[packetStartIndex + 22], parentFrame->Data()[packetStartIndex + 23]};
        index = packetStartIndex + 32;
        if (index < packetEndIndex) {
            comment = Utils::ByteConverter::ReadNullTerminatedString(parentFrame->Data(), parentFrame->DataLength(), index, false, false, 43);
        }
    } else if (command == static_cast<uint8_t>(BrowserMessageOpCode::RequestElection)) {
        uptimeSeconds = Utils::ByteConverter::ToUInt32(parentFrame->Data(), packetStartIndex + 6, true);
        hasUptime = true;
        int index = packetStartIndex + 14;
        hostname = Utils::ByteConverter::ReadNullTerminatedString(parentFrame->Data(), parentFrame->DataLength(), index, false, false, 16);
    }
}

QString CifsBrowserPacket::CommandName() const {
    switch (static_cast<BrowserMessageOpCode>(command)) {
        case BrowserMessageOpCode::HostAnnouncement: return "HostAnnouncement";
        case BrowserMessageOpCode::AnnouncementRequest: return "AnnouncementRequest";
        case BrowserMessageOpCode::RequestElection: return "RequestElection";
        case BrowserMessageOpCode::GetBackupListRequest: return "GetBackupListRequest";
        case BrowserMessageOpCode::GetBackupListResponse: return "GetBackupListResponse";
        case BrowserMessageOpCode::BecomeBackup: return "BecomeBackup";
        case BrowserMessageOpCode::DomainAnnouncement: return "DomainAnnouncement";
        case BrowserMessageOpCode::MasterAnnouncement: return "MasterAnnouncement";
        case BrowserMessageOpCode::ResetStateRequest: return "ResetStateRequest";
        case BrowserMessageOpCode::LocalMasterAnnouncement: return "LocalMasterAnnouncement";
        default: return QString();
    }
}

std::vector<AbstractPacket*> CifsBrowserPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
