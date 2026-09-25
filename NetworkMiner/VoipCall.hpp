#pragma once
#include <QString>
#include <QDateTime>
#include <memory>
#include <vector>

namespace PacketHandlerFramework {
namespace FileTransfer {
class ReconstructedFile;
}
}

namespace PacketParser {
class NetworkHost;
}

namespace NetworkMiner {

class VoipCall {
public:
    static const QString FORMAT_PREFIX;

    QString From;
    QString To;
    QDateTime Start;
    QDateTime End;
    QString CallId;
    std::shared_ptr<PacketParser::NetworkHost> SourceHost;
    std::shared_ptr<PacketParser::NetworkHost> DestinationkHost;
    uint8_t Encoding;
    QString EncodingString;
    QString WavFilePath;

    std::shared_ptr<PacketHandlerFramework::FileTransfer::ReconstructedFile> reconstructedFile;

    VoipCall();
    VoipCall(const QString& from, const QString& to, const QDateTime& start, const QDateTime& end, const QString& callId, std::shared_ptr<PacketParser::NetworkHost> source, std::shared_ptr<PacketParser::NetworkHost> destination, uint8_t encoding, const QString& encodingString, const QString& wavFilePath);

    virtual ~VoipCall() = default;

    void OnWavFileReconstructed(const QString& extendedFileId, std::shared_ptr<PacketHandlerFramework::FileTransfer::ReconstructedFile> file);
};

} // namespace NetworkMiner
