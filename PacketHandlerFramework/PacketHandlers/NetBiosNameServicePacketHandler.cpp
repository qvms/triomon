#include "NetBiosNameServicePacketHandler.hpp"
#include "../../PacketParser/Packets/ITransportLayerPacket.hpp"
#include "../../PacketHandlerFramework/MainPacketHandler.hpp"
#include "../../NetworkMiner/Events/ParametersEventArgs.hpp"
#include "../../PacketParser/Utils/ByteConverter.hpp"

#include <QList>

namespace PacketHandlerFramework {
namespace PacketHandlers {

NetBiosNameServicePacketHandler::NetBiosNameServicePacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
    // do nothing more
}

QList<std::type_index> NetBiosNameServicePacketHandler::getParsedTypes() const {
    return { typeid(PacketParser::Packets::NetBiosNameServicePacket) };
}

void NetBiosNameServicePacketHandler::ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<PacketParser::Packets::AbstractPacket*>& packetList) {
    PacketParser::Packets::ITransportLayerPacket* transportLayerPacket = nullptr;

    for (auto* p : packetList) {
        if (auto* nbnsPacket = dynamic_cast<PacketParser::Packets::NetBiosNameServicePacket*>(p)) {
            ExtractData(nbnsPacket, sourceHost, destinationHost, transportLayerPacket);
        } else if (auto* tlp = dynamic_cast<PacketParser::Packets::ITransportLayerPacket*>(p)) {
            transportLayerPacket = tlp;
        }
    }
}

void NetBiosNameServicePacketHandler::ExtractData(PacketParser::Packets::NetBiosNameServicePacket* netBiosNameServicePacket, NetworkHost* sourceHost, NetworkHost* destinationHost, PacketParser::Packets::ITransportLayerPacket* transportLayerPacket) {
    QList<QPair<QString, QString>> parameters;

    // QueriedNetBiosName property getter needed (mocking logic)
    // if (!netBiosNameServicePacket->QueriedNetBiosName().isEmpty()) {
    //     sourceHost->AddQueriedNetBiosName(netBiosNameServicePacket->QueriedNetBiosName());
    //     parameters.append({"NetBIOS Query", netBiosNameServicePacket->QueriedNetBiosName()});
    // }

    // Logic based on answers & additional records:
    // This part requires access to properties like AnswerResourceRecords that we assume
    // are available or mocked appropriately for C++.

    if (!parameters.isEmpty() && transportLayerPacket != nullptr) {
        /*
        if (netBiosNameServicePacket->Flags().Response()) {
            GetMainPacketHandler()->OnParametersDetected(
                NetworkMiner::Events::ParametersEventArgs(
                    netBiosNameServicePacket->ParentFrame()->FrameNumber(), 
                    sourceHost, destinationHost, 
                    transportLayerPacket->TransportProtocol(), 
                    transportLayerPacket->SourcePort(), 
                    transportLayerPacket->DestinationPort(), 
                    parameters, 
                    netBiosNameServicePacket->ParentFrame()->Timestamp(), 
                    "NBNS Response"
                )
            );
        }
        */
    }
}

void NetBiosNameServicePacketHandler::Reset() {
    // empty
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
