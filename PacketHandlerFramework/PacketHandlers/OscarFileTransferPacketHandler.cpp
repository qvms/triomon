#include "OscarFileTransferPacketHandler.hpp"

void OscarFileTransferPacketHandler::extractData(const QByteArray& packetData, const NetworkHost& sourceHost, const NetworkHost& destinationHost, const QDateTime& timestamp, quint16 sourcePort, quint16 destinationPort, bool isTcp, int frameNumber, const NetworkTcpSession* tcpSession) {
    if(packetData.isEmpty()) return;
    
    // We would try to parse the OscarFileTransferPacket here
    // OscarFileTransferPacket oscarPacket(packetData, ...);
    // and extract parameters...
}

void OscarFileTransferPacketHandler::extractData(const ITcpFlowInfo* tcpFlowInfo, const NetworkHost& sourceHost, const NetworkHost& destinationHost, const QDateTime& timestamp, quint16 sourcePort, quint16 destinationPort, bool isTcp, int frameNumber, const NetworkTcpSession* tcpSession) {
    // Handling TCP flow reconstruction
}
