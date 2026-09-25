#pragma once

#include "IPortProtocolFinder.hpp"
#include "IUdpPayloadProtocolFinder.hpp"
#include "ApplicationLayerProtocol.hpp"
#include "Packets/IIPPacket.hpp"
#include <memory>
#include <cstdint>

namespace PacketParser {

class UdpPortProtocolFinder : public IPortProtocolFinder {
private:
    static std::shared_ptr<IPortProtocolFinder> instance;
    static std::shared_ptr<IUdpPayloadProtocolFinder> pipiInstance;

public:
    UdpPortProtocolFinder() = default;
    virtual ~UdpPortProtocolFinder() = default;

    static std::shared_ptr<IPortProtocolFinder> getInstance();
    static void setInstance(std::shared_ptr<IPortProtocolFinder> value);

    static std::shared_ptr<IUdpPayloadProtocolFinder> getPipiInstance();
    static void setPipiInstance(std::shared_ptr<IUdpPayloadProtocolFinder> value);

    static bool isSipPort(uint16_t port);

    ApplicationLayerProtocol getApplicationLayerProtocol(Packets::RFC1700Protocol transport, uint16_t sourcePort, uint16_t destinationPort) override;
};

} // namespace PacketParser
