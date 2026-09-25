#include "McNmfPacketHandler.hpp"
#include "../../NetworkMiner/FileStreamAssembler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

McNmfPacketHandler::McNmfPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> McNmfPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::McNmfPacket>() };
}

ApplicationLayerProtocol McNmfPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::McNmf;
}

int McNmfPacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    int parsedBytes = 0;
    std::shared_ptr<McNmfSessionEncoding> encoding;
    if (sessionEncodings.contains(tcpSession)) {
        encoding = sessionEncodings[tcpSession];
    }

    for (const auto& p : packetList) {
        if (auto mcNmfPacket = std::dynamic_pointer_cast<PacketParser::Packets::McNmfPacket>(p)) {
            parsedBytes += mcNmfPacket->ParsedBytesCount();
            if (mcNmfPacket->MessageId() == 0x01) {
                if (!encoding) {
                    encoding = std::make_shared<McNmfSessionEncoding>();
                    sessionEncodings.insert(tcpSession, encoding);
                }
                
                QMap<QString, QString> nvc;
                QMap<QString, QString> attributes;
                QString body;

                QByteArray data = mcNmfPacket->ParentFrame()->GetData();
                int offset = mcNmfPacket->PacketStartIndex() + mcNmfPacket->ParsedBytesCount();
                int endIndex = mcNmfPacket->PacketEndIndex();
                bool foundEncoding = false;
                
                while (offset + 1 < endIndex) {
                    int stringLength = PacketParser::Utils::ByteConverter::ToUInt16(data, offset, false);
                    offset += 2;
                    if (offset + stringLength > endIndex + 1) break;
                    QString key = QString::fromLatin1(data.mid(offset, stringLength));
                    offset += stringLength;

                    if (offset + 1 > endIndex) break;
                    stringLength = PacketParser::Utils::ByteConverter::ToUInt16(data, offset, false);
                    offset += 2;
                    if (offset + stringLength > endIndex + 1) break;
                    QString value = QString::fromLatin1(data.mid(offset, stringLength));
                    offset += stringLength;
                    
                    if (key == "E") {
                        foundEncoding = true;
                        encoding->Encoding = value;
                    }
                    nvc.insert(key, value);
                    attributes.insert(key, value);
                    body += key + "=" + value + "\r\n";
                }

                if (foundEncoding) {
                    MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                        mcNmfPacket->ParentFrame()->GetFrameNumber(), tcpSession->GetFlow()->FiveTuple(),
                        transferIsClientToServer, nvc, mcNmfPacket->ParentFrame()->GetTimestamp(), "MCNMF Message"
                    ));
                    
                    MainPacketHandler()->OnMessageDetected(Events::MessageEventArgs(
                        ApplicationLayerProtocol::McNmf,
                        tcpSession->GetSourceHost(transferIsClientToServer),
                        tcpSession->GetDestinationHost(transferIsClientToServer),
                        mcNmfPacket->ParentFrame()->GetFrameNumber(),
                        mcNmfPacket->ParentFrame()->GetTimestamp(),
                        QString(),
                        "McNMF Message 0x01",
                        body,
                        QString(),
                        attributes,
                        mcNmfPacket->PacketLength()
                    ));
                }

            } else if (mcNmfPacket->MessageId() == 0x04) {
                if (encoding) {
                    if (ParseFileData(tcpSession, transferIsClientToServer, mcNmfPacket, encoding->Encoding)) {
                        parsedBytes += mcNmfPacket->PacketLength() - mcNmfPacket->ParsedBytesCount();
                    }
                }
            }
        }
    }
    return parsedBytes;
}

bool McNmfPacketHandler::ParseFileData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, std::shared_ptr<PacketParser::Packets::McNmfPacket> mcNmfPacket, const QString& encoding) {
    if (encoding != "UTF-8" && encoding != "UTF-16") {
        return false;
    }
    
    QByteArray data = mcNmfPacket->ParentFrame()->GetData();
    int offset = mcNmfPacket->PacketStartIndex() + mcNmfPacket->ParsedBytesCount();
    int endIndex = mcNmfPacket->PacketEndIndex();
    
    if (offset + 1 > endIndex) return false;
    int filenameLength = PacketParser::Utils::ByteConverter::ToUInt16(data, offset, false);
    offset += 2;
    if (offset + filenameLength > endIndex + 1) return false;
    
    QString filename;
    if (encoding == "UTF-8") {
        filename = QString::fromUtf8(data.mid(offset, filenameLength));
    } else {
        filename = QString::fromUtf16(reinterpret_cast<const char16_t*>(data.constData() + offset), filenameLength / 2);
    }
    offset += filenameLength;

    if (offset + 7 > endIndex) return false;
    quint64 fileSize = PacketParser::Utils::ByteConverter::ToUInt64(data, offset, false);
    offset += 8;

    if (offset + 1 > endIndex) return false;
    int detailsLength = PacketParser::Utils::ByteConverter::ToUInt16(data, offset, false);
    offset += 2;
    if (offset + detailsLength > endIndex + 1) return false;
    
    QString fileDetails;
    if (encoding == "UTF-8") {
        fileDetails = QString::fromUtf8(data.mid(offset, detailsLength));
    } else {
        fileDetails = QString::fromUtf16(reinterpret_cast<const char16_t*>(data.constData() + offset), detailsLength / 2);
    }
    
    auto assembler = std::make_shared<NetworkMiner::FileStreamAssembler>(
        MainPacketHandler()->FileStreamAssemblerList(),
        tcpSession->GetFlow()->FiveTuple(),
        transferIsClientToServer,
        NetworkMiner::FileStreamTypes::MC_NMF,
        filename,
        "/",
        fileDetails,
        mcNmfPacket->ParentFrame()->GetFrameNumber(),
        mcNmfPacket->ParentFrame()->GetTimestamp()
    );
    assembler->SetFileContentLength(fileSize);
    assembler->SetFileSegmentRemainingBytes(fileSize);
    
    if (assembler->TryActivate()) {
        MainPacketHandler()->FileStreamAssemblerList()->Add(assembler);
        return true;
    }
    return false;
}

void McNmfPacketHandler::Reset() {
    sessionEncodings.clear();
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
