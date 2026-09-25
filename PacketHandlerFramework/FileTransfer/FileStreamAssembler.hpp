#include "PacketHandlerFramework/FileTransfer/FileStreamTypes.hpp"
#include "PacketHandlerFramework/FileTransfer/ReconstructedFile.hpp"
#pragma once
#include <QString>
#include <QHostAddress>
#include <QDateTime>
#include "../../PacketParser/Packets/HttpPacket.hpp"
#include "FileStreamTypes.hpp"
#include "ReconstructedFile.hpp"

// Forward declaration
namespace PacketParser { namespace Packets { class TcpPacket; } }
using namespace PacketParser::Packets;
using namespace PacketHandlerFramework::FileTransfer;

class FileStreamAssemblerList;

class FileStreamAssembler {
public:
    FileStreamAssembler(FileStreamAssemblerList* parentAssemblerList, const QHostAddress& sourceHost, quint16 sourcePort, const QHostAddress& destinationHost, quint16 destinationPort, bool tcpTransfer, FileStreamTypes fileStreamType, const QString& filename, const QString& fileLocation, const QString& details, int initialFrameNumber, const QDateTime& timestamp);
    FileStreamAssembler(FileStreamAssemblerList* parentAssemblerList, const QHostAddress& sourceHost, quint16 sourcePort, const QHostAddress& destinationHost, quint16 destinationPort, bool tcpTransfer, FileStreamTypes fileStreamType, const QString& filename, const QString& fileLocation, int fileContentLength, int fileSegmentRemainingBytes, const QString& details, const QString& extendedFileId, int initialFrameNumber, const QDateTime& timestamp);

    // Getters and Setters
    QHostAddress getSourceIp() const { return sourceIp; }
    QHostAddress getDestinationIp() const { return destinationIp; }
    quint16 getSourcePort() const { return sourcePort; }
    quint16 getDestinationPort() const { return destinationPort; }
    QString getFilename() const { return filename; }
    QString getFileLocation() const { return fileLocation; }
    QString getDetails() const { return details; }
    void setDetails(const QString& val) { details = val; }
    bool getTcpTransfer() const { return tcpTransfer; }
    int getInitialFrameNumber() const { return initialFrameNumber; }
    QDateTime getTimestamp() const { return timestamp; }
    QString getExtendedFileId() const { return extendedFileId; }
    
    FileStreamTypes getFileStreamType() const { return fileStreamType; }
    void setFileStreamType(FileStreamTypes val) { fileStreamType = val; }
    HttpPacket::ContentEncodings getContentEncoding() const { return contentEncoding; }
    void setContentEncoding(HttpPacket::ContentEncodings val) { contentEncoding = val; }

    ReconstructedFile* getReconstructedFile() { return reconstructedFile; }
    void setReconstructedFile(ReconstructedFile* file) { reconstructedFile = file; }

    void addData(const QByteArray& data);
    void addData(const TcpPacket& tcpPacket);

    void Flush();
    void ExtractToCache(const QString& cacheDirectory);

    static QString getFilePath(bool tempCachePath, bool tcpTransfer, const QHostAddress& sourceIp, const QHostAddress& destinationIp, quint16 sourcePort, quint16 destinationPort, FileStreamTypes fileStreamType, const QString& fileLocation, const QString& filename, FileStreamAssemblerList* parentAssemblerList);

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
    HttpPacket::ContentEncodings contentEncoding;
    int initialFrameNumber;
    QDateTime timestamp;
    QString extendedFileId;

    QByteArray buffer;
    ReconstructedFile* reconstructedFile = nullptr;
};
