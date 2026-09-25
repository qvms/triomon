#pragma once

#include "AbstractPacketHandler.hpp"
#include "../PacketParser/Packets/RtpPacket.hpp"
#include "../PacketParser/Packets/UdpPacket.hpp"
#include "../PacketHandler.hpp"
#include "../../NetworkMiner/AudioStream.hpp"
#include <QMap>
#include <tuple>
#include <QHostAddress>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class RtpPacketHandler : public AbstractPacketHandler {
public:
    enum class RtpPayloadType : quint8 {
        G711_PCM_U = 0,
        G711_PCM_A = 8,
        G722 = 9,
        G729 = 18
    };

    explicit RtpPacketHandler(PacketHandler* mainPacketHandler);

    QList<int> ParsedTypes() const override;

    void ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) override;
    void Reset() override;

private:
    using StreamKey = std::tuple<quint32, quint16, quint32, quint16, quint8>;
    QMap<StreamKey, std::shared_ptr<NetworkMiner::AudioStream>> audioStreams;
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
