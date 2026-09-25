#pragma once
#include <QString>
#include <QHostAddress>
#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QHash>
#include "IOsFingerprinterInfo.hpp"
#include "DeviceFingerprint.hpp"
#include "../../PacketParser/Packets/AbstractPacket.hpp"

namespace PacketHandlerFramework {
namespace Fingerprints {
class IOsFingerprinter : public IOsFingerprinterInfo {
public:
    virtual ~IOsFingerprinter() = default;

    virtual bool TryGetOperatingSystems(QList<DeviceFingerprint>& osList, const QList<PacketParser::Packets::AbstractPacket*>& packetList) = 0;
};

} // namespace Fingerprints
} // namespace PacketHandlerFramework
