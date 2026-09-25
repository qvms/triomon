#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <QSet>
#include <vector>

namespace PacketParser {
namespace Packets {

class FtpPacket : public AbstractPacket {
private:
    static const QSet<QString> userCommandSet;

    bool clientToServer;
    QString requestCommand;
    QString requestArgument;
    int responseCode;
    QString responseArgument;

    static bool TryReadToResponseCode(int expectedResponseCode, const uint8_t* data, size_t dataLength, int& index, std::vector<QString>& lines);

    FtpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);

public:
    static bool ContainsLikelyFtpBanner(const QString& line);
    static bool IsLikelyFtpStart(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, uint16_t sourcePort);
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, uint16_t sourcePort, AbstractPacket*& ftpPacket);

    bool ClientToServer() const { return clientToServer; }
    QString RequestCommand() const { return requestCommand; }
    QString RequestArgument() const { return requestArgument; }
    int ResponseCode() const { return responseCode; }
    QString ResponseArgument() const { return responseArgument; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
