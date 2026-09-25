#include "PacketHandlerWrapper.hpp"

namespace NetworkMiner {
    
    PacketHandlerWrapper::PacketHandlerWrapper(QObject* parentForm, bool useRelativePathIfAvailable, QObject* parent)
    : QObject(parent), parentForm(parentForm) {
        packetHandler = std::make_shared<PacketParser::PacketHandler>();
    }
    
} // namespace NetworkMiner
