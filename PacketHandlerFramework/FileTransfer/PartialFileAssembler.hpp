#pragma once
#include <QString>
#include <QHostAddress>
#include <QDateTime>
#include "FileStreamTypes.hpp"
#include "../../PacketParser/Packets/HttpPacket.hpp"

class FileStreamAssemblerList;

class PartialFileAssembler {
public:
    PartialFileAssembler(FileStreamAssemblerList* parentAssemblerList, const QHostAddress& sourceHost, quint16 sourcePort, const QHostAddress& destinationHost, quint16 destinationPort, bool tcpTransfer, FileStreamTypes fileStreamType, const QString& filename, const QString& fileLocation, const QString& details, int initialFrameNumber, const QDateTime& timestamp);

    // Getters
    QHostAddress getSourceIp() const { return sourceIp; }
    QHostAddress getDestinationIp() const { return destinationIp; }
    quint16 getSourcePort() const { return sourcePort; }
    quint16 getDestinationPort() const { return destinationPort; }
    QString getFilename() const { return filename; }
    QString getFileLocation() const { return fileLocation; }
    QString getDetails() const { return details; }
    bool getTcpTransfer() const { return tcpTransfer; }
    FileStreamTypes getFileStreamType() const { return fileStreamType; }
    int getInitialFrameNumber() const { return initialFrameNumber; }
    QDateTime getTimestamp() const { return timestamp; }

    void addData(const QByteArray& data);

private:
    QHostAddress sourceIp;
    QHostAddress destinationIp;
    quint16 sourcePort;
    quint16 destinationPort;
    QString filename;
    QString fileLocation;
    QString details;
    bool tcpTransfer;
    FileStreamTypes fileStreamType;
    int initialFrameNumber;
    QDateTime timestamp;

    QByteArray buffer;
};
