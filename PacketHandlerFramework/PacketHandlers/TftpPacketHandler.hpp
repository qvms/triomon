#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/Packets/TftpPacket.hpp"
#include "../PacketParser/Packets/UdpPacket.hpp"
#include "../PacketHandler.hpp"
#include "../../NetworkMiner/FileStreamAssembler.hpp"
#include <QMap>
#include <QString>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class TftpPacketHandler : public AbstractPacketHandler {
public:
    explicit TftpPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;

    void ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    QMap<QString, int> tftpSessionBlksizeList;

    bool TryGetTftpFileStreamAssembler(std::shared_ptr<NetworkMiner::FileStreamAssembler>& assembler, PacketParser::FileStreamAssemblerList* fileStreamAssemblerList, NetworkHost* sourceHost, quint16 sourcePort, NetworkHost* destinationHost, quint16 destinationPort);
    bool TryCreateNewAssembler(std::shared_ptr<NetworkMiner::FileStreamAssembler>& assembler, PacketParser::FileStreamAssemblerList* fileStreamAssemblerList, std::shared_ptr<PacketParser::Packets::TftpPacket> tftpPacket, NetworkHost* sourceHost, quint16 sourcePort, NetworkHost* destinationHost);
    QString GetTftpSessionId(NetworkHost* sourceHost, quint16 sourcePort, NetworkHost* destinationHost, quint16 destinationPort);
    void ExtractFileData(std::shared_ptr<NetworkMiner::FileStreamAssembler> assembler, PacketParser::FileStreamAssemblerList* fileStreamAssemblerList, NetworkHost* sourceHost, quint16 sourcePort, NetworkHost* destinationHost, quint16 destinationPort, std::shared_ptr<PacketParser::Packets::TftpPacket> tftpPacket);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
