#pragma once

#include "PacketReceivedEventArgs.hpp"

namespace NetworkWrapper {

class ISniffer {
public:
    virtual ~ISniffer() = default;
    virtual PacketReceivedEventArgs::PacketTypes GetBasePacketType() const = 0;
    virtual void StartSniffing() = 0;
    virtual void StopSniffing() = 0;
};

} // namespace NetworkWrapper
