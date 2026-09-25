#pragma once

#include "AbstractPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class McNmfPacketHandler : public AbstractPacketHandler {
public:
    McNmfPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~McNmfPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class MeterpreterPacketHandler : public AbstractPacketHandler {
public:
    MeterpreterPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~MeterpreterPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class NetBiosDatagramServicePacketHandler : public AbstractPacketHandler {
public:
    NetBiosDatagramServicePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~NetBiosDatagramServicePacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class NetBiosNameServicePacketHandler : public AbstractPacketHandler {
public:
    NetBiosNameServicePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~NetBiosNameServicePacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class NetBiosSessionServicePacketHandler : public AbstractPacketHandler {
public:
    NetBiosSessionServicePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~NetBiosSessionServicePacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
