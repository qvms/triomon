#include "MultiplePacketHandlers2.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

// FTP
FtpPacketHandler::FtpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void FtpPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void FtpPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void FtpPacketHandler::Reset() { }

// GenericShim
GenericShimPacketHandler::GenericShimPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void GenericShimPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void GenericShimPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void GenericShimPacketHandler::Reset() { }

// HpSwitchProtocol
HpSwitchProtocolPacketHandler::HpSwitchProtocolPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void HpSwitchProtocolPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void HpSwitchProtocolPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void HpSwitchProtocolPacketHandler::Reset() { }

// HTTP
HttpPacketHandler::HttpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void HttpPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void HttpPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void HttpPacketHandler::Reset() { }

// LPD
LpdPacketHandler::LpdPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {}

void LpdPacketHandler::ExtractDataFromPacket(void* transportPacket) { }
void LpdPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const std::vector<void*>& packetList) { }
void LpdPacketHandler::Reset() { }

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
