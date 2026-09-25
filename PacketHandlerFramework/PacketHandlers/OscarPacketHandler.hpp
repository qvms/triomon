#ifndef OSCARPACKETHANDLER_HPP
#define OSCARPACKETHANDLER_HPP

#include "../../PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "../../PacketParser/PacketHandler.hpp"
#include <QObject>

class OscarPacketHandler : public ITcpSessionPacketHandler {
    Q_OBJECT
public:
    OscarPacketHandler(PacketHandler* packetHandler) : ITcpSessionPacketHandler(packetHandler) {}
    
    void extractData(const QByteArray& packetData, const NetworkHost& sourceHost, const NetworkHost& destinationHost, const QDateTime& timestamp, quint16 sourcePort, quint16 destinationPort, bool isTcp, int frameNumber, const NetworkTcpSession* tcpSession) override;
    void extractData(const ITcpFlowInfo* tcpFlowInfo, const NetworkHost& sourceHost, const NetworkHost& destinationHost, const QDateTime& timestamp, quint16 sourcePort, quint16 destinationPort, bool isTcp, int frameNumber, const NetworkTcpSession* tcpSession) override;
};

#endif // OSCARPACKETHANDLER_HPP
