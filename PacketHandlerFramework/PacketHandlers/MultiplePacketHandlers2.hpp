#pragma once

#include "AbstractPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class FtpPacketHandler : public AbstractPacketHandler {
public:
    FtpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~FtpPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class GenericShimPacketHandler : public AbstractPacketHandler {
public:
    GenericShimPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~GenericShimPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class HpSwitchProtocolPacketHandler : public AbstractPacketHandler {
public:
    HpSwitchProtocolPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~HpSwitchProtocolPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class HttpPacketHandler : public AbstractPacketHandler {
public:
    HttpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~HttpPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

class LpdPacketHandler : public AbstractPacketHandler {
public:
    LpdPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler);
    virtual ~LpdPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override;
    void ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList);
    void Reset();
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
