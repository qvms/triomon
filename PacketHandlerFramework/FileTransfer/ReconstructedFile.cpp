#include "ReconstructedFile.hpp"
#include <QFileInfo>
#include <QLocale>

namespace PacketHandlerFramework {
namespace FileTransfer {

ReconstructedFile::ReconstructedFile(const QString& path, std::shared_ptr<PacketParser::NetworkHost> sourceHost, 
                                     std::shared_ptr<PacketParser::NetworkHost> destinationHost, uint16_t sourcePort, 
                                     uint16_t destinationPort, bool tcpTransfer, FileStreamTypes fileStreamType, 
                                     const QString& details, int initialFrameNumber, const QDateTime& timestamp)
    : path(path), details(details), sourceHost(sourceHost), destinationHost(destinationHost),
      sourcePort(sourcePort), destinationPort(destinationPort), tcpTransfer(tcpTransfer),
      fileStreamType(fileStreamType), initialFrameNumber(initialFrameNumber), timestamp(timestamp) {
    
    QFileInfo fi(path);
    filename = fi.fileName();
    if (fi.exists()) {
        fileSize = fi.size();
    } else {
        fileSize = 0;
    }
}

QString ReconstructedFile::GetFileEnding() const {
    int idx = filename.lastIndexOf('.');
    if (idx < 0 || idx == filename.length() - 1) {
        return "";
    }
    return filename.mid(idx + 1).toLower();
}

QString ReconstructedFile::SourcePortString() const {
    return QString("%1 %2").arg(tcpTransfer ? "TCP" : "UDP").arg(sourcePort);
}

QString ReconstructedFile::DestinationPortString() const {
    return QString("%1 %2").arg(tcpTransfer ? "TCP" : "UDP").arg(destinationPort);
}

QString ReconstructedFile::FileSizeString() const {
    QLocale locale;
    return locale.toString(static_cast<qlonglong>(fileSize)) + " B";
}

bool ReconstructedFile::IsImage() const {
    QString ext = GetFileEnding();
    return ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "png" || 
           ext == "bmp" || ext == "tif" || ext == "tiff";
}

bool ReconstructedFile::IsIcon() const {
    return GetFileEnding() == "ico";
}

bool ReconstructedFile::IsMultipartFormData() const {
    return GetFileEnding() == "mime";
}

} // namespace FileTransfer
} // namespace PacketHandlerFramework
