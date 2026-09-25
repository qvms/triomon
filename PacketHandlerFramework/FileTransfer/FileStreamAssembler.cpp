#include "FileStreamAssembler.hpp"
#include "FileStreamAssemblerList.hpp"
#include "../../PacketParser/PacketHandler.hpp"
#include "ReconstructedFile.hpp"
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <QRegularException> // or QRegularExpression in Qt 5+

FileStreamAssembler::FileStreamAssembler(FileStreamAssemblerList* parentAssemblerList, const QHostAddress& sourceHost, quint16 sourcePort, const QHostAddress& destinationHost, quint16 destinationPort, bool tcpTransfer, FileStreamTypes fileStreamType, const QString& filename, const QString& fileLocation, const QString& details, int initialFrameNumber, const QDateTime& timestamp)
    : FileStreamAssembler(parentAssemblerList, sourceHost, sourcePort, destinationHost, destinationPort, tcpTransfer, fileStreamType, filename, fileLocation, 0, 0, details, QString(), initialFrameNumber, timestamp)
{
}

FileStreamAssembler::FileStreamAssembler(FileStreamAssemblerList* parentAssemblerList, const QHostAddress& sourceHost, quint16 sourcePort, const QHostAddress& destinationHost, quint16 destinationPort, bool tcpTransfer, FileStreamTypes fileStreamType, const QString& filename, const QString& fileLocation, int fileContentLength, int fileSegmentRemainingBytes, const QString& details, const QString& extendedFileId, int initialFrameNumber, const QDateTime& timestamp)
    : parentAssemblerList(parentAssemblerList),
      sourceHost(sourceHost),
      destinationHost(destinationHost),
      sourcePort(sourcePort),
      destinationPort(destinationPort),
      tcpTransfer(tcpTransfer),
      fileStreamType(fileStreamType),
      contentEncoding(HttpPacket::ContentEncodings::Identity),
      filename(filename),
      fileLocation(fileLocation),
      fileContentLength(fileContentLength),
      fileSegmentRemainingBytes(fileSegmentRemainingBytes),
      details(details),
      extendedFileId(extendedFileId),
      assembledByteCount(0),
      fileStream(nullptr),
      isActive(false),
      initialFrameNumber(initialFrameNumber),
      timestamp(timestamp)
{
    fixFilenameAndLocation(this->filename, this->fileLocation);
    if(isActive) {
        fileStream = new QFile(getFilePath(true));
        fileStream->open(QIODevice::ReadWrite);
    }
}

void FileStreamAssembler::setContentEncoding(HttpPacket::ContentEncodings val) {
    contentEncoding = val;
    // We would need to implement decompression checking here, stubbed
    if (!filename.endsWith(".gz")) {
        // ... Check if parentAssemblerList->DecompressGzipStreams
    }
}

void FileStreamAssembler::fixFilenameAndLocation(QString& filename, QString& fileLocation) {
    if(filename.contains('/')) {
        fileLocation = fileLocation + filename.left(filename.lastIndexOf('/') + 1);
        filename = filename.mid(filename.lastIndexOf('/') + 1);
    }
    if(filename.contains('\\')) {
        fileLocation = fileLocation + filename.left(filename.lastIndexOf('\\') + 1);
        filename = filename.mid(filename.lastIndexOf('\\') + 1);
    }

    filename = QUrl::fromPercentEncoding(filename.toUtf8());
    
    QList<QChar> specialCharacters = {':', '*', '?', '"', '<', '>', '|'};
    for (QChar c : specialCharacters) {
        filename.replace(c, "");
    }
    filename.replace("/", "");
    filename.replace("\\", "");
    
    while(filename.startsWith('.')) {
        filename = filename.mid(1);
    }
    if(filename.length() > 32) {
        int extensionPosition = filename.lastIndexOf('.');
        if(extensionPosition < 0 || extensionPosition <= filename.length() - 20) {
            filename = filename.left(20);
        } else {
            filename = filename.left(20 - filename.length() + extensionPosition) + filename.mid(extensionPosition);
        }
    }

    fileLocation = QUrl::fromPercentEncoding(fileLocation.toUtf8());
    fileLocation.replace('\\', '/');
    for (QChar c : specialCharacters) {
        fileLocation.replace(c, "");
    }
    if(!fileLocation.isEmpty() && !fileLocation.startsWith('/')) {
        fileLocation = "/" + fileLocation;
    }
    if(fileLocation.endsWith('/')) {
        fileLocation.chop(1);
    }
    if(fileLocation.length() > 40) {
        fileLocation = fileLocation.left(40);
    }
}

bool FileStreamAssembler::tryActivate() {
    try {
        if(!fileStream) {
            fileStream = new QFile(getFilePath(true));
            fileStream->open(QIODevice::ReadWrite);
        }
        isActive = true;
        return true;
    } catch(...) {
        return false;
    }
}

QString FileStreamAssembler::getFilePath(bool tempCachePath) const {
    return getFilePath(tempCachePath, tcpTransfer, sourceHost, destinationHost, sourcePort, destinationPort, fileStreamType, fileLocation, filename, parentAssemblerList);
}

QString FileStreamAssembler::getFilePath(bool tempCachePath, bool tcpTransfer, const QHostAddress& sourceIp, const QHostAddress& destinationIp, quint16 sourcePort, quint16 destinationPort, FileStreamTypes fileStreamType, const QString& fileLocation, const QString& filename, FileStreamAssemblerList* parentAssemblerList) {
    QString filePath;
    QString protocolString;
    
    if(fileStreamType == FileStreamTypes::HttpGetNormal || fileStreamType == FileStreamTypes::HttpGetChunked) protocolString = "HTTP";
    else if(fileStreamType == FileStreamTypes::SMB) protocolString = "SMB";
    else if(fileStreamType == FileStreamTypes::TFTP) protocolString = "TFTP";
    else if(fileStreamType == FileStreamTypes::TlsCertificate) protocolString = "TLS_Cert";
    else if(fileStreamType == FileStreamTypes::FTP) protocolString = "FTP";
    else if(fileStreamType == FileStreamTypes::HttpPostMimeMultipartFormData) protocolString = "MIME_form-data";
    else if(fileStreamType == FileStreamTypes::HttpPostMimeFileData) protocolString = "MIME_file-data";
    else protocolString = "UNKNOWN";

    QString transportString = tcpTransfer ? "TCP" : "UDP";

    QString safeLocation = fileLocation;
    safeLocation.replace("..", "_");

    if(tempCachePath) {
        filePath = QString("cache/%1_%2%3 - %4_%5%6_%7.txt").arg(sourceIp.toString(), transportString, QString::number(sourcePort), destinationIp.toString(), transportString, QString::number(destinationPort), protocolString);
    } else {
        filePath = QString("%1/%2 - %3 %4%5/%6").arg(sourceIp.toString(), protocolString, transportString, QString::number(sourcePort), safeLocation, filename);
    }
    
    // In a real implementation we would fetch the folder path from parentAssemblerList
    // filePath = parentAssemblerList->getFileOutputFolder() + QDir::separator() + filePath;
    filePath = "output/" + filePath;

    if(!tempCachePath && QFile::exists(filePath)) {
        int iterator = 1;
        QString filePathPrefix;
        QString filePathSuffix;
        int extensionPosition = filePath.lastIndexOf('.');
        int filenamePosition = filePath.lastIndexOf(QDir::separator());

        if(extensionPosition < 0) {
            filePathPrefix = filePath;
        } else if(extensionPosition > filenamePosition) {
            filePathPrefix = filePath.left(extensionPosition);
            filePathSuffix = filePath.mid(extensionPosition);
        } else {
            filePathPrefix = filePath;
        }

        QString uniqueFilePath = QString("%1[%2]%3").arg(filePathPrefix).arg(iterator).arg(filePathSuffix);
        while(QFile::exists(uniqueFilePath)) {
            iterator++;
            uniqueFilePath = QString("%1[%2]%3").arg(filePathPrefix).arg(iterator).arg(filePathSuffix);
        }
        filePath = uniqueFilePath;
    }

    return filePath;
}

void FileStreamAssembler::setRemainingBytesInFile(int remainingByteCount) {
    fileContentLength = assembledByteCount + remainingByteCount;
}

void FileStreamAssembler::addData(const TcpPacket& tcpPacket) {
    if(!tcpTransfer) return;
    if(tcpPacket.getPayloadDataLength() > 0) {
        addData(tcpPacket.getTcpPacketPayloadData(), tcpPacket.getSequenceNumber());
    }
}

void FileStreamAssembler::addData(const QByteArray& packetData, quint16 packetNumber) {
    addData(packetData, static_cast<quint32>(packetNumber));
}

void FileStreamAssembler::addData(const QByteArray& packetData, quint32 tcpPacketSequenceNumber) {
    if(!isActive) return;
    if(packetData.isEmpty()) return;
    if(tcpPacketBufferWindow.contains(tcpPacketSequenceNumber)) return;
    
    if(fileStreamType != FileStreamTypes::HttpGetChunked && fileStreamType != FileStreamTypes::TFTP && fileContentLength != -1) {
        if(fileSegmentRemainingBytes < packetData.size()) {
            return;
        }
        fileSegmentRemainingBytes -= packetData.size();
    }
    
    tcpPacketBufferWindow.insert(tcpPacketSequenceNumber, packetData);
    assembledByteCount += packetData.size();

    while(tcpPacketBufferWindow.size() > 64) {
        quint32 key = tcpPacketBufferWindow.firstKey();
        fileStream->write(tcpPacketBufferWindow.value(key));
        tcpPacketBufferWindow.remove(key);
    }
    
    if((fileStreamType == FileStreamTypes::HttpGetNormal || fileStreamType == FileStreamTypes::SMB || fileStreamType == FileStreamTypes::TlsCertificate || fileStreamType == FileStreamTypes::FTP || fileStreamType == FileStreamTypes::HttpPostMimeMultipartFormData || fileStreamType == FileStreamTypes::HttpPostMimeFileData) && assembledByteCount >= fileContentLength && fileContentLength != -1) {
        finishAssembling();
    } else if(fileStreamType != FileStreamTypes::HttpGetChunked && fileStreamType != FileStreamTypes::TFTP && fileSegmentRemainingBytes == 0) {
        isActive = false;
    } else if(fileStreamType == FileStreamTypes::HttpGetChunked) {
        QByteArray chunkTrailer;
        chunkTrailer.append((char)0x30).append((char)0x0d).append((char)0x0a).append((char)0x0d).append((char)0x0a);
        if(packetData.size() >= chunkTrailer.size()) {
            if(packetData.endsWith(chunkTrailer)) {
                finishAssembling();
            }
        }
    }
}

void FileStreamAssembler::finishAssembling() {
    isActive = false;
    for(const QByteArray& data : tcpPacketBufferWindow) {
        fileStream->write(data);
    }
    fileStream->flush();
    tcpPacketBufferWindow.clear();
    
    // In a real implementation we would call parentAssemblerList->remove(this, false)
    
    QString destinationPath = getFilePath(false);
    QString directoryName = QFileInfo(destinationPath).path();
    QDir().mkpath(directoryName);

    if(QFile::exists(destinationPath)) {
        QFile::remove(destinationPath);
    }

    if(fileStreamType == FileStreamTypes::HttpGetChunked || contentEncoding == HttpPacket::ContentEncodings::Gzip || contentEncoding == HttpPacket::ContentEncodings::Deflate) {
        fileStream->seek(0);
        // Implement decompression/dechunking logic here, omitted for brevity
        fileStream->close();
        QFile::remove(getFilePath(true));
    } else if(fileStreamType == FileStreamTypes::HttpPostMimeMultipartFormData) {
        // Implement MIME parsing
        fileStream->close();
        QFile::remove(getFilePath(true));
    } else {
        fileStream->close();
        QFile::rename(getFilePath(true), destinationPath);
    }
}

void FileStreamAssembler::writeStreamToFile(QIODevice* stream, const QString& destinationPath) {
    QFile outputFile(destinationPath);
    if(outputFile.open(QIODevice::WriteOnly)) {
        outputFile.write(stream->readAll());
        outputFile.close();
    }
}

void FileStreamAssembler::clear() {
    tcpPacketBufferWindow.clear();
    if(fileStream) {
        fileStream->close();
        QFile::remove(fileStream->fileName());
        delete fileStream;
        fileStream = nullptr;
    }
}
