#include "UdpPortProtocolFinder.hpp"

namespace PacketParser {

std::shared_ptr<IPortProtocolFinder> UdpPortProtocolFinder::instance = nullptr;
std::shared_ptr<IUdpPayloadProtocolFinder> UdpPortProtocolFinder::pipiInstance = nullptr;

std::shared_ptr<IPortProtocolFinder> UdpPortProtocolFinder::getInstance() {
    if (!instance) {
        instance = std::make_shared<UdpPortProtocolFinder>();
    }
    return instance;
}

void UdpPortProtocolFinder::setInstance(std::shared_ptr<IPortProtocolFinder> value) {
    instance = value;
}

std::shared_ptr<IUdpPayloadProtocolFinder> UdpPortProtocolFinder::getPipiInstance() {
    return pipiInstance;
}

void UdpPortProtocolFinder::setPipiInstance(std::shared_ptr<IUdpPayloadProtocolFinder> value) {
    pipiInstance = value;
}

bool UdpPortProtocolFinder::isSipPort(uint16_t port) {
    if (port == 5060)
        return true;
    if (port >= 5060 && port <= 5082)
        return true;
    if (port >= 5196 && port <= 5199)
        return true;

    return false;
}

ApplicationLayerProtocol UdpPortProtocolFinder::getApplicationLayerProtocol(Packets::RFC1700Protocol transport, uint16_t sourcePort, uint16_t destinationPort) {
    if (destinationPort == 53 || sourcePort == 53 || destinationPort == 5353 || sourcePort == 5353 || destinationPort == 5355 || sourcePort == 5355) {
        return ApplicationLayerProtocol::DNS;
    }
    else if (destinationPort == 67 || destinationPort == 68 || sourcePort == 67 || sourcePort == 68) {
        return ApplicationLayerProtocol::DHCP;
    }
    else if (destinationPort == 69 || sourcePort == 69) {
        return ApplicationLayerProtocol::TFTP;
    }
    else if (destinationPort == 88 || sourcePort == 88) {
        return ApplicationLayerProtocol::Kerberos;
    }
    else if (destinationPort == 137 || sourcePort == 137) {
        return ApplicationLayerProtocol::NetBiosNameService;
    }
    else if (destinationPort == 138 || sourcePort == 138) {
        return ApplicationLayerProtocol::NetBiosDatagramService;
    }
    else if (destinationPort == 161 || sourcePort == 161) {
        return ApplicationLayerProtocol::SNMP;
    }
    else if (destinationPort == 443 || sourcePort == 443) {
        return ApplicationLayerProtocol::QUIC;
    }
    else if (destinationPort == 514 || sourcePort == 514) {
        return ApplicationLayerProtocol::Syslog;
    }
    else if (destinationPort == 853 || sourcePort == 853) {
        return ApplicationLayerProtocol::QUIC;
    }
    else if (destinationPort == 1900 || sourcePort == 1900) {
        return ApplicationLayerProtocol::UPnP;
    }
    else if (destinationPort == 2123 || sourcePort == 2123) {
        return ApplicationLayerProtocol::GTP;
    }
    else if (destinationPort == 2152 || sourcePort == 2152) {
        return ApplicationLayerProtocol::GTP;
    }
    else if (destinationPort == 3544 || sourcePort == 3544) {
        return ApplicationLayerProtocol::Teredo;
    }
    else if (destinationPort == 4789 || sourcePort == 4789 || destinationPort == 8472 || sourcePort == 8472) {
        return ApplicationLayerProtocol::VXLAN;
    }
    else if (destinationPort == 5246 || sourcePort == 5246 || destinationPort == 5247 || sourcePort == 5247) {
        return ApplicationLayerProtocol::CAPWAP;
    }
    else if (destinationPort == 37008 || sourcePort == 37008 || destinationPort == 5247 || sourcePort == 5247) {
        return ApplicationLayerProtocol::TZSP;
    }
    else if (isSipPort(destinationPort) || isSipPort(sourcePort)) {
        return ApplicationLayerProtocol::SIP;
    }
    else {
        return ApplicationLayerProtocol::Unknown;
    }
}

} // namespace PacketParser
