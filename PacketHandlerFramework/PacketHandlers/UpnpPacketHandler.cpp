#include "UpnpPacketHandler.hpp"
#include "../../PacketParser/NetworkHost.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

UpnpPacketHandler::UpnpPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
    this->userAgentFingerprinter.Name = "UPnP USER-AGENT";
}

QList<int> UpnpPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::UpnpPacket>() };
}

void UpnpPacketHandler::ExtractData(NetworkHost* sourceHost, NetworkHost* /*destinationHost*/, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    for (const auto& p : packetList) {
        if (!p) continue;
        
        auto upnpPacket = std::dynamic_pointer_cast<PacketParser::Packets::UpnpPacket>(p);
        if (upnpPacket) {
            ExtractData(upnpPacket.get(), sourceHost);
        }
    }
}

void UpnpPacketHandler::ExtractData(PacketParser::Packets::UpnpPacket* upnpPacket, NetworkHost* sourceHost) {
    if (!upnpPacket || upnpPacket->FieldList.isEmpty()) {
        return;
    }

    if (!sourceHost->HasUniversalPlugAndPlayFieldList()) {
        sourceHost->InitializeUniversalPlugAndPlayFieldList();
    }

    for (const QString& field : upnpPacket->FieldList) {
        if (!sourceHost->ContainsUniversalPlugAndPlayField(field)) {
            sourceHost->AddUniversalPlugAndPlayField(field, field);
            
            if (field.startsWith("USER-AGENT:", Qt::CaseInsensitive)) {
                QString userAgent = field.mid(11).trimmed();
                if (!userAgent.isEmpty()) {
                    sourceHost->AddHttpUserAgentBanner(userAgent);
                    QString osID;
                    if (UserAgentFingerprinter::TryExtractUserAgentOS(userAgent, osID)) {
                        // Assuming sourceHost has AddProbableOs signature handling IOsFingerprinter ptr and double
                        sourceHost->AddProbableOs(osID, &this->userAgentFingerprinter, 0.5);
                    }
                }
            }
        }
    }
}

void UpnpPacketHandler::Reset() {
    // throw new Exception("The method or operation is not implemented.");
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
