#include "PartialFileAssembler.hpp"

PartialFileAssembler::PartialFileAssembler(FileStreamAssemblerList* parentAssemblerList, const QHostAddress& sourceHost, quint16 sourcePort, const QHostAddress& destinationHost, quint16 destinationPort, bool tcpTransfer, FileStreamTypes fileStreamType, const QString& filename, const QString& fileLocation, const QString& details, int initialFrameNumber, const QDateTime& timestamp)
    : parentAssemblerList(parentAssemblerList),
      sourceHost(sourceHost),
      destinationHost(destinationHost),
      sourcePort(sourcePort),
      destinationPort(destinationPort),
      tcpTransfer(tcpTransfer),
      fileStreamType(fileStreamType),
      filename(filename),
      fileLocation(fileLocation),
      details(details),
      initialFrameNumber(initialFrameNumber),
      timestamp(timestamp)
{
}

void PartialFileAssembler::addData(const QByteArray& packetData, quint32 tcpPacketSequenceNumber) {
    if(packetData.isEmpty()) return;
    if(!tcpPacketBufferWindow.contains(tcpPacketSequenceNumber)) {
        tcpPacketBufferWindow.insert(tcpPacketSequenceNumber, packetData);
    }
}

void PartialFileAssembler::clear() {
    tcpPacketBufferWindow.clear();
}
