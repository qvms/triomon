#ifndef OSCARFILETRANSFERPACKETHANDLER_HPP
#define OSCARFILETRANSFERPACKETHANDLER_HPP

#include "../../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../../PacketParser/Packets/OscarFileTransferPacket.hpp"
#include "../../PacketParser/PacketHandler.hpp"
#include <QObject>

class OscarFileTransferPacketHandler : public ITcpSessionPacketHandler {
    Q_OBJECT
public:
    OscarFileTransferPacketHandler(PacketHandler* packetHandler) : ITcpSessionPacketHandler(packetHandler) {}
    
    void extractData(const QByteArray& packetData, const NetworkHost& sourceHost, const NetworkHost& destinationHost, const QDateTime& timestamp, quint16 sourcePort, quint16 destinationPort, bool isTcp, int frameNumber, const NetworkTcpSession* tcpSession) override;
    void extractData(const ITcpFlowInfo* tcpFlowInfo, const NetworkHost& sourceHost, const NetworkHost& destinationHost, const QDateTime& timestamp, quint16 sourcePort, quint16 destinationPort, bool isTcp, int frameNumber, const NetworkTcpSession* tcpSession) override;
};

#endif // OSCARFILETRANSFERPACKETHANDLER_HPP
