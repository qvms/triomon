#pragma once

#include "AbstractPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class CotpPacketHandler : public AbstractPacketHandler {
public:
    CotpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~CotpPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class DhcpPacketHandler : public AbstractPacketHandler {
public:
    DhcpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~DhcpPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class DnsPacketHandler : public AbstractPacketHandler {
public:
    DnsPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~DnsPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
