#pragma once

#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../../PacketParser/Packets/HttpPacket.hpp"
#include "../../PacketParser/Packets/TcpPacket.hpp"
#include "../../PacketParser/NetworkTcpSession.hpp"
#include "../../PacketParser/NetworkHost.hpp"
#include <memory>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class HttpPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
public:
    HttpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~HttpPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    
    // ITcpSessionPacketHandler
    int ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset();

    bool TryExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList);

private:
    void ExtractHttpData(std::shared_ptr<PacketParser::Packets::HttpPacket> httpPacket, std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket, std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
