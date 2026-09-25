#include "NetworkHost.hpp"
#include <sstream>

namespace PacketParser {

NetworkHost::NetworkHost(const QHostAddress& ipAddress) : ipAddress(ipAddress) {
    sentPackets = std::make_shared<PacketHandlerFramework::NetworkPacketList>();
    receivedPackets = std::make_shared<PacketHandlerFramework::NetworkPacketList>();
}

std::string NetworkHost::ToString() const {
    if (!hostNameList.isEmpty()) {
        return hostNameList.first().toStdString() + " (" + ipAddress.toString().toStdString() + ")";
    }
    return ipAddress.toString().toStdString();
}

} // namespace PacketParser
