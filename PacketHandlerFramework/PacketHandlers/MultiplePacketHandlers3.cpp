#include "MultiplePacketHandlers3.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

// McNmf
McNmfPacketHandler::McNmfPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void McNmfPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void McNmfPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void McNmfPacketHandler::Reset() { }

// Meterpreter
MeterpreterPacketHandler::MeterpreterPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void MeterpreterPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void MeterpreterPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void MeterpreterPacketHandler::Reset() { }

// NetBios Datagram Service
NetBiosDatagramServicePacketHandler::NetBiosDatagramServicePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void NetBiosDatagramServicePacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void NetBiosDatagramServicePacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void NetBiosDatagramServicePacketHandler::Reset() { }

// NetBios Name Service
NetBiosNameServicePacketHandler::NetBiosNameServicePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void NetBiosNameServicePacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void NetBiosNameServicePacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void NetBiosNameServicePacketHandler::Reset() { }

// NetBios Session Service
NetBiosSessionServicePacketHandler::NetBiosSessionServicePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void NetBiosSessionServicePacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void NetBiosSessionServicePacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void NetBiosSessionServicePacketHandler::Reset() { }


} // namespace PacketHandlers
} // namespace PacketHandlerFramework
