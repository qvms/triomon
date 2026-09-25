#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <vector>

namespace PacketParser {
namespace Packets {

class CifsBrowserPacket : public AbstractPacket {
public:
    enum class BrowserMessageOpCode : uint8_t {
        HostAnnouncement = 0x01,
        AnnouncementRequest = 0x02,
        RequestElection = 0x08,
        GetBackupListRequest = 0x09,
        GetBackupListResponse = 0x0A,
        BecomeBackup = 0x0B,
        DomainAnnouncement = 0x0C,
        MasterAnnouncement = 0x0D,
        ResetStateRequest = 0x0E,
        LocalMasterAnnouncement = 0x0F
    };

private:
    uint8_t command;
    std::pair<uint8_t, uint8_t> osVersion;
    QString domainOrWorkgroup;
    QString hostname;
    QString comment;
    uint32_t uptimeSeconds;
    bool hasUptime;

public:
    CifsBrowserPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint8_t Command() const { return command; }
    QString CommandName() const;
    std::pair<uint8_t, uint8_t> OSVersion() const { return osVersion; }
    QString DomainOrWorkgroup() const { return domainOrWorkgroup; }
    QString Hostname() const { return hostname; }
    QString Comment() const { return comment; }
    bool HasUptime() const { return hasUptime; }
    uint32_t UptimeSeconds() const { return uptimeSeconds; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
