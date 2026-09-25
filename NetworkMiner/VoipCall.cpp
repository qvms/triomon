#include "VoipCall.hpp"

namespace NetworkMiner {

const QString VoipCall::FORMAT_PREFIX = "ITU-T G.711 PCMA / ";

VoipCall::VoipCall() : Encoding(0) {}

VoipCall::VoipCall(const QString& from, const QString& to, const QDateTime& start, const QDateTime& end, const QString& callId, std::shared_ptr<PacketParser::NetworkHost> source, std::shared_ptr<PacketParser::NetworkHost> destination, uint8_t encoding, const QString& encodingString, const QString& wavFilePath)
    : From(from), To(to), Start(start), End(end), CallId(callId), SourceHost(source), DestinationkHost(destination), Encoding(encoding), EncodingString(encodingString), WavFilePath(wavFilePath) {
    if (this->EncodingString.startsWith(FORMAT_PREFIX)) {
        this->EncodingString = this->EncodingString.mid(FORMAT_PREFIX.length());
    }
}

void VoipCall::OnWavFileReconstructed(const QString& extendedFileId, std::shared_ptr<PacketHandlerFramework::FileTransfer::ReconstructedFile> file) {
    this->reconstructedFile = file;
}

} // namespace NetworkMiner
