#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include <QString>
#include <vector>

namespace PacketParser {
namespace Packets {

class TabularDataStreamPacket : public AbstractPacket, public virtual ISessionPacket {
public:
    enum class PacketTypes : uint8_t {
        SqlQuery = 1,
        PreTds7Login = 2,
        RemoteProcedureCall = 3,
        TableResponse = 4,
        AttentionSignal = 6,
        BulkLoadData = 7,
        TransactionManagerRequest = 14,
        Tds7Login = 16,
        SspiMessage = 17,
        PreLoginMessage = 18
    };

private:
    uint8_t packetType;
    bool isLastPacket;
    uint16_t packetSize;

    QString query;
    QString clientHostname;
    QString username;
    QString password;
    QString appname;
    QString serverHostname;
    QString libraryName;
    QString databaseName;

public:
    TabularDataStreamPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    uint8_t PacketType() const { return packetType; }
    bool IsLastPacket() const { return isLastPacket; }
    uint16_t PacketSize() const { return packetSize; }

    QString Query() const { return query; }
    QString ClientHostname() const { return clientHostname; }
    QString Username() const { return username; }
    QString Password() const { return password; }
    QString AppName() const { return appname; }
    QString ServerHostname() const { return serverHostname; }
    QString LibraryName() const { return libraryName; }
    QString DatabaseName() const { return databaseName; }

    bool PacketHeaderIsComplete() const override { return PacketLength() >= packetSize; }
    int ParsedBytesCount() const override { return PacketLength(); }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
