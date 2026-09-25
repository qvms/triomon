#include "NetworkPacket.hpp"

namespace PacketHandlerFramework {

NetworkPacket::NetworkPacket(std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, void* ipPacket)
    : sourceHost(sourceHost), destinationHost(destinationHost), tcpSynFlag(false), tcpSynAckFlag(false), tcpPacketByteCount(0), packetBytes(0) {
    // Stub implementation as we don't have full AbstractPacket mock yet
}

void NetworkPacket::SetTcpData(void* tcpPacket) {
    // Stub implementation
}

void NetworkPacket::SetUdpData(void* udpPacket) {
    // Stub implementation
}

} // namespace PacketHandlerFramework
