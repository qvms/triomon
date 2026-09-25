#include "MultiplePacketHandlers.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

// COTP
CotpPacketHandler::CotpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void CotpPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void CotpPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void CotpPacketHandler::Reset() { }

// DHCP
DhcpPacketHandler::DhcpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void DhcpPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void DhcpPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void DhcpPacketHandler::Reset() { }

// DNS
DnsPacketHandler::DnsPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void DnsPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void DnsPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void DnsPacketHandler::Reset() { }

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
