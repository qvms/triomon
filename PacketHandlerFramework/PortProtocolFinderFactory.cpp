#include "PortProtocolFinderFactory.hpp"
#include "NetworkFlow.hpp"
#include "TcpSessionProtocolFinder.hpp"
#include <stdexcept>

PortProtocolFinderFactory::PortProtocolFinderFactory(PacketHandler* packetHandler)
    : packetHandler(packetHandler)
{
}

bool PortProtocolFinderFactory::getPortIndependentProtocolIdentificationEnabled() const {
    return false;
}

void PortProtocolFinderFactory::setPortIndependentProtocolIdentificationEnabled(bool /*enabled*/) {
    throw std::runtime_error("PIPI is not supported in the free version of NetworkMiner");
}

ISessionProtocolFinder* PortProtocolFinderFactory::createProtocolFinder(NetworkFlow* flow, qint64 startFrameNumber) {
    if (flow && flow->getFiveTuple().getTransport() == FiveTuple::TransportProtocol::TCP) {
        return new TcpSessionProtocolFinder(flow, startFrameNumber, packetHandler);
    } else {
        throw std::runtime_error("There is only a protocol finder for TCP");
    }
}

void PortProtocolFinderFactory::reset() {
}
