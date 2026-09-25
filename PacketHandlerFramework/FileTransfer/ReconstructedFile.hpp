#pragma once

#include <QString>
#include <QDateTime>
#include <memory>
#include "PacketParser/NetworkHost.hpp"
#include "PacketHandlerFramework/FileTransfer/FileStreamTypes.hpp"

namespace PacketHandlerFramework {
namespace FileTransfer {

class ReconstructedFile {
private:
    QString path;
    QString details;
    std::shared_ptr<PacketParser::NetworkHost> sourceHost;
    std::shared_ptr<PacketParser::NetworkHost> destinationHost;
    uint16_t sourcePort;
    uint16_t destinationPort;
    bool tcpTransfer;
    FileStreamTypes fileStreamType;
    long long fileSize;
    QString filename;
    int initialFrameNumber;
    QDateTime timestamp;

    QString GetFileEnding() const;

public:
    ReconstructedFile(const QString& path, std::shared_ptr<PacketParser::NetworkHost> sourceHost, 
                      std::shared_ptr<PacketParser::NetworkHost> destinationHost, uint16_t sourcePort, 
                      uint16_t destinationPort, bool tcpTransfer, FileStreamTypes fileStreamType, 
                      const QString& details, int initialFrameNumber, const QDateTime& timestamp);

    QString FilePath() const { return path; }
    std::shared_ptr<PacketParser::NetworkHost> SourceHost() const { return sourceHost; }
    QString SourcePortString() const;
    std::shared_ptr<PacketParser::NetworkHost> DestinationHost() const { return destinationHost; }
    QString DestinationPortString() const;
    QString Filename() const { return filename; }
    QString FileSizeString() const;
    QString Details() const { return details; }
    FileStreamTypes FileStreamType() const { return fileStreamType; }
    int InitialFrameNumber() const { return initialFrameNumber; }
    QDateTime Timestamp() const { return timestamp; }

    bool IsImage() const;
    bool IsIcon() const;
    bool IsMultipartFormData() const;
};

} // namespace FileTransfer
} // namespace PacketHandlerFramework
