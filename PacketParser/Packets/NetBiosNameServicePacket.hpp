#pragma once
#include "NetBiosPacket.hpp"
#include "DnsPacket.hpp"
#include <vector>

namespace PacketParser {
namespace Packets {

class NetBiosNameServicePacket : public NetBiosPacket {
public:
    class ResourceRecord {
    private:
        QString name;
        uint16_t type;
        uint16_t recordClass;
        uint32_t ttl;
        std::vector<uint8_t> data;

    public:
        ResourceRecord(NetBiosNameServicePacket* nbnsPacket, int& offset);

        QString Name() const { return name; }
        QString NameTrimmed() const;
        uint16_t Type() const { return type; }
        uint16_t Class() const { return recordClass; }
        uint32_t TTL() const { return ttl; }
        const std::vector<uint8_t>& Data() const { return data; }
    };

private:
    uint16_t transactionID;
    DnsPacket::HeaderFlags flags;
    uint16_t questionCount;
    uint16_t answerCount;
    uint16_t authorityCount;
    uint16_t additionalCount;

    QString questionNameDecoded;
    uint16_t questionType;
    uint16_t questionClass;

    std::vector<ResourceRecord*> answerResourceRecords;
    std::vector<ResourceRecord*> authorityResourceRecords;
    std::vector<ResourceRecord*> additionalResourceRecords;

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& netBiosNameServicePacket);

    NetBiosNameServicePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    ~NetBiosNameServicePacket();

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

    DnsPacket::HeaderFlags Flags() const { return flags; }
};

} // namespace Packets
} // namespace PacketParser
