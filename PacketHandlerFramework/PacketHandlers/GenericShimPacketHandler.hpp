#pragma once

#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../../PacketParser/Packets/AbstractPacket.hpp"
#include "../../PacketParser/Packets/ISessionPacket.hpp"
#include "../../PacketParser/NetworkTcpSession.hpp"
#include "../../PacketParser/ApplicationLayerProtocol.hpp"
#include "../../PacketParser/Frame.hpp"
#include <QList>
#include <QMap>
#include <memory>
#include <typeinfo>

namespace PacketHandlerFramework {
namespace PacketHandlers {

template<typename T>
class GenericShimPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::ITcpSessionPacketHandler {
    static_assert(std::is_base_of<PacketParser::Packets::AbstractPacket, T>::value, "T must derive from AbstractPacket");
    static_assert(std::is_base_of<PacketParser::Packets::ISessionPacket, T>::value, "T must derive from ISessionPacket");

private:
    PacketParser::ApplicationLayerProtocol handledProtocol;

public:
    GenericShimPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler, PacketParser::ApplicationLayerProtocol handledProtocol)
        : AbstractPacketHandler(mainPacketHandler), handledProtocol(handledProtocol) {
    }

    virtual ~GenericShimPacketHandler() = default;

    void ExtractDataFromPacket(void* transportPacket) override {
        // Implementation expected through ITcpSessionPacketHandler
    }

    PacketParser::ApplicationLayerProtocol HandledProtocol() const {
        return handledProtocol;
    }

    int ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override {
        std::shared_ptr<T> shimPacket = nullptr;
        const PacketParser::Frame* originalFrame = nullptr;
        QMap<int, std::shared_ptr<PacketParser::Packets::AbstractPacket>> sortedPacketList;

        for (const auto& p : packetList) {
            if (shimPacket != nullptr) {
                sortedPacketList.insert(p->GetPacketStartIndex(), p);
            }
            if (originalFrame == nullptr) {
                originalFrame = p->GetParentFrame();
            }
            if (!shimPacket) {
                shimPacket = std::dynamic_pointer_cast<T>(p);
            }
        }

        if (shimPacket != nullptr && originalFrame != nullptr) {
            if (!sortedPacketList.isEmpty()) {
                QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>> newPacketList = sortedPacketList.values();
                // Frame clone logic - simplified
                std::shared_ptr<PacketParser::Frame> innerFrame = originalFrame->CloneWithPacketList(newPacketList);
                GetMainPacketHandler()->ParseFrame(innerFrame.get()); 
            }
            return shimPacket->GetParsedBytesCount();
        }

        return 0;
    }

    void Reset() {
        // do nothing
    }
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
