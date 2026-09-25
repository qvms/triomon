#include "TlsRecordPacketHandler.hpp"
#include "../../SharedUtils/Logger.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

TlsRecordPacketHandler::TlsRecordPacketHandler(PacketHandler* mainPacketHandler, const QMap<QString, std::shared_ptr<Fingerprints::IJa4Fingerprint>>& ja4Fingerprints, bool verifyX509Certificates)
    : AbstractTlsHandshakePacketHandler(mainPacketHandler, ja4Fingerprints, verifyX509Certificates) {
}

QList<int> TlsRecordPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::TlsRecordPacket>() };
}

ApplicationLayerProtocol TlsRecordPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::SSL;
}

int TlsRecordPacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    bool successfulExtraction = false;
    std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket;
    
    for (const auto& p : packetList) {
        if (auto tcp = std::dynamic_pointer_cast<PacketParser::Packets::TcpPacket>(p)) {
            tcpPacket = tcp;
            break;
        }
    }
    
    int parsedBytes = 0;
    if (tcpPacket) {
        for (const auto& p : packetList) {
            if (auto tlsRecordPacket = std::dynamic_pointer_cast<PacketParser::Packets::TlsRecordPacket>(p)) {
                if (tlsRecordPacket->TlsRecordIsComplete()) {
                    QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket>>* recordList = nullptr;
                    
                    if (tlsRecordFragmentCache.contains(tcpSession->GetFlow()->FiveTuple())) {
                        if (transferIsClientToServer) {
                            recordList = &tlsRecordFragmentCache[tcpSession->GetFlow()->FiveTuple()].first;
                        } else {
                            recordList = &tlsRecordFragmentCache[tcpSession->GetFlow()->FiveTuple()].second;
                        }
                        if (recordList && !recordList->isEmpty() && recordList->first()->ContentType() != tlsRecordPacket->ContentType()) {
                            recordList->clear();
                        }
                    } else {
                        if (transferIsClientToServer) {
                            tlsRecordFragmentCache.insert(tcpSession->GetFlow()->FiveTuple(), qMakePair(QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket>>(), QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket>>()));
                            recordList = &tlsRecordFragmentCache[tcpSession->GetFlow()->FiveTuple()].first;
                        } else {
                            tlsRecordFragmentCache.insert(tcpSession->GetFlow()->FiveTuple(), qMakePair(QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket>>(), QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket>>()));
                            recordList = &tlsRecordFragmentCache[tcpSession->GetFlow()->FiveTuple()].second;
                        }
                    }
                    
                    if (recordList) {
                        recordList->append(tlsRecordPacket);
                    }
                    
                    if (recordList) {
                        if (tlsRecordPacket->ContentType() == PacketParser::Packets::TlsRecordPacket::ContentTypes::Handshake) {
                            int parsedHandshakesTotalLength = 0;
                            QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket::HandshakePacket>> handshakes;
                            // handshakes = PacketParser::Packets::TlsRecordPacket::HandshakePacket::GetHandshakes(*recordList);
                            
                            for (const auto& handshake : handshakes) {
                                parsedHandshakesTotalLength += handshake->PacketLength();
                                // ExtractHandshakeData(tcpPacket, tcpSession->GetFlow()->FiveTuple(), transferIsClientToServer, handshake);
                            }
                            
                            RemoveParsedTlsRecordsFromList(*recordList, parsedHandshakesTotalLength, tcpPacket);
                        } else if (tlsRecordPacket->ContentType() == PacketParser::Packets::TlsRecordPacket::ContentTypes::Application) {
                            recordList->clear();
                        } else if (recordList->size() > 3) {
                            recordList->clear();
                        }
                    }
                    successfulExtraction = true;
                    parsedBytes += tlsRecordPacket->Length() + 5;
                } else if (tlsRecordPacket->Length() > 16384) {
                    successfulExtraction = true;
                    parsedBytes = tcpPacket->PayloadDataLength();
                }
            }
        }
    }
    
    if (successfulExtraction) {
        return parsedBytes;
    }
    return 0;
}

void TlsRecordPacketHandler::RemoveParsedTlsRecordsFromList(QList<std::shared_ptr<PacketParser::Packets::TlsRecordPacket>>& recordList, int parsedBytes, std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket) {
    if (parsedBytes > 0) {
        int accumulatedRecordLength = 0;
        for (int i = 0; i < recordList.size(); i++) {
            accumulatedRecordLength += recordList[i]->Length();
            if (accumulatedRecordLength >= parsedBytes) {
                if (accumulatedRecordLength > parsedBytes) {
                    SharedUtils::Logger::Log("TLS data boundary is not on a TLS record boundary in frame " + QString::number(tcpPacket->ParentFrame()->GetFrameNumber()), SharedUtils::Logger::EventLogEntryType::Warning);
                }
                recordList.remove(0, i + 1);
                break;
            }
        }
    }
}

void TlsRecordPacketHandler::Reset() {
    tlsRecordFragmentCache.clear();
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
