#pragma once
#include <QString>
#include <QHostAddress>
#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QHash>

namespace PacketParser {
class ISessionProtocolFinder {
public:
    virtual ~ISessionProtocolFinder() = default;

    virtual PacketParser::ApplicationLayerProtocol GetConfirmedApplicationLayerProtocol() = 0;
    virtual void SetConfirmedApplicationLayerProtocol(PacketParser::ApplicationLayerProtocol value, bool setAsPersistantProtocolOnServerEndPoint) = 0;
    virtual QList<PacketParser::ApplicationLayerProtocol> GetProbableApplicationLayerProtocols() = 0;
};

} // namespace PacketParser
