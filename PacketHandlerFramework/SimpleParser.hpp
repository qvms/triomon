#pragma once
#include <memory>
#include "PacketHandler.hpp"

namespace SharedUtils { namespace Pcap { class IPcapFrame; } }

namespace PacketHandlerFramework {

class SimpleParser {
private:
    std::shared_ptr<MainPacketHandler> mainPacketHandler;

public:
    SimpleParser(std::shared_ptr<MainPacketHandler> mainPacketHandler)
        : mainPacketHandler(mainPacketHandler) {
    }

    std::shared_ptr<MainPacketHandler> get_MainPacketHandler() const {
        return mainPacketHandler;
    }

    void ParseFrame(std::shared_ptr<SharedUtils::Pcap::IPcapFrame> frame) {
        if (mainPacketHandler) {
            mainPacketHandler->ParsePcapFrame(frame);
        }
    }
};

} // namespace PacketHandlerFramework
