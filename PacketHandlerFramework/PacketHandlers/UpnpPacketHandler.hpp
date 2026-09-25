#pragma once
#include "AbstractPacketHandler.hpp"
#include "../../PacketParser/PacketHandlers/IPacketHandler.hpp"
#include "../../PacketParser/Packets/UpnpPacket.hpp"
#include "../../PacketParser/Fingerprints/IOsFingerprinter.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

class UpnpPacketHandler : public AbstractPacketHandler, public PacketParser::PacketHandlers::IPacketHandler {
private:
    class UserAgentFingerprinter : public PacketParser::Fingerprints::IOsFingerprinter {
    public:
        QString Name;
        
        static bool TryExtractUserAgentOS(const QString& userAgent, QString& osID) {
            // Simplified logic: basic OS extraction based on common User-Agent strings
            if (userAgent.contains("Windows NT 10.0", Qt::CaseInsensitive)) { osID = "Windows 10"; return true; }
            if (userAgent.contains("Windows NT 6.3", Qt::CaseInsensitive)) { osID = "Windows 8.1"; return true; }
            if (userAgent.contains("Windows NT 6.2", Qt::CaseInsensitive)) { osID = "Windows 8"; return true; }
            if (userAgent.contains("Windows NT 6.1", Qt::CaseInsensitive)) { osID = "Windows 7"; return true; }
            if (userAgent.contains("Mac OS X", Qt::CaseInsensitive)) { osID = "Mac OS X"; return true; }
            if (userAgent.contains("Linux", Qt::CaseInsensitive)) { osID = "Linux"; return true; }
            if (userAgent.contains("Android", Qt::CaseInsensitive)) { osID = "Android"; return true; }
            return false;
        }
    };

    UserAgentFingerprinter userAgentFingerprinter;

public:
    explicit UpnpPacketHandler(PacketHandler* mainPacketHandler);
    virtual ~UpnpPacketHandler() = default;

    QList<int> ParsedTypes() const override;

    void ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    void ExtractData(PacketParser::Packets::UpnpPacket* upnpPacket, NetworkHost* sourceHost);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
