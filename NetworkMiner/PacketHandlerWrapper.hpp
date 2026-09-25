#pragma once

#include "PacketParser/PacketHandler.hpp"
#include <memory>
#include <QObject>

namespace NetworkMiner {
    
    class PacketHandlerWrapper : public QObject {
        Q_OBJECT
    private:
        QObject* parentForm;
        std::shared_ptr<PacketParser::PacketHandler> packetHandler;
        
    public:
        PacketHandlerWrapper(QObject* parentForm, bool useRelativePathIfAvailable = true, QObject* parent = nullptr);
        virtual ~PacketHandlerWrapper() = default;
        
        std::shared_ptr<PacketParser::PacketHandler> PacketHandler() const { return packetHandler; }
    };
    
} // namespace NetworkMiner
