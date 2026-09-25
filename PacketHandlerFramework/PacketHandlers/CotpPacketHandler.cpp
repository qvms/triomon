#include "CotpPacketHandler.hpp"
#include "../../PacketHandlerFramework/Events/ParametersEventArgs.hpp"
#include "../../PacketHandlerFramework/MainPacketHandler.hpp"
#include "../../PacketParser/Utils/ByteConverter.hpp"
#include "../../PacketParser/Utils/StringManglerUtil.hpp"
#include <QVariantMap>

namespace PacketHandlerFramework {
namespace PacketHandlers {

CotpPacketHandler::CotpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

void CotpPacketHandler::ExtractDataFromPacket(void* transportPacket) {
    // Expected to be handled via the ITcpSessionPacketHandler interface 
}

int CotpPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    
    std::shared_ptr<PacketParser::Packets::CotpPacket> cotpPacket = nullptr;
    for(auto& p : packetList) {
        cotpPacket = std::dynamic_pointer_cast<PacketParser::Packets::CotpPacket>(p);
        if(cotpPacket) {
            break;
        }
    }

    if(cotpPacket) {
        QVariantMap parms;

        if(!cotpPacket->EndOfTsdu()) {
            std::vector<uint8_t> data;
            if(cotpPacket->TryGetTsduData(data)) {
                if(!data.empty()) {
                    // QMutexLocker in Qt6 equivalent or just assume single threaded context for now if porting 
                    QByteArray ba(reinterpret_cast<const char*>(data.data()), data.size());
                    cotpDataSegments[tcpSession].append(ba);
                }
            }
        }
        else if (cotpPacket->GetTpdu() == PacketParser::Packets::CotpPacket::Tpdu::Data) {
            if(cotpDataSegments.contains(tcpSession)) {
                QByteArray data = cotpDataSegments.take(tcpSession);
                std::vector<uint8_t> lastData;
                if(cotpPacket->TryGetTsduData(lastData)) {
                    data.append(reinterpret_cast<const char*>(lastData.data()), lastData.size());
                    // TODO: make sense of this data!
                }
            }
        }

        QString details = "TPKT/COTP data";
        // 0 == RDP from C# enum
        if (cotpPacket->EncapsulatedProtocol == 0 /* CotpPacket.PayloadProtocol.RDP */) {
            PacketParser::Packets::CotpPacket::Tpdu tpdu = cotpPacket->GetTpdu();
            if (tpdu == PacketParser::Packets::CotpPacket::Tpdu::ConnectionRequest) {
                int cotpPayloadIndex;
                if(cotpPacket->TryGetVariablePartIndex(cotpPayloadIndex)) {
                    QString rdpCookie;
                    if (TryGetRdpCookie(cotpPacket->GetParentFrame()->GetData(), cotpPayloadIndex, rdpCookie)) {
                        details = "RDP Cookie";
                        if(!rdpCookie.isEmpty() && rdpCookie != "mstshash=") {
                             auto cred = std::make_shared<PacketHandlerFramework::NetworkCredential>(
                                    tcpSession->GetClientHost(), tcpSession->GetServerHost(),
                                    "RDP Cookie", rdpCookie, "", cotpPacket->GetParentFrame()->GetTimestamp());
                             GetMainPacketHandler()->AddCredential(cred);
                        }
                        if(rdpCookie.contains('=')) {
                            QStringList parts = rdpCookie.split('=');
                            if (parts.size() > 1) {
                                QString pn = parts[0].trimmed();
                                QString pv = parts[1].trimmed();
                                if(!pn.isEmpty() && !pv.isEmpty()) {
                                    parms.insert(pn, pv);
                                }
                            }
                        }
                    }
                }
            } else if (tpdu == PacketParser::Packets::CotpPacket::Tpdu::ConnectionConfirm) {
                int cotpPayloadIndex;
                if(cotpPacket->TryGetVariablePartIndex(cotpPayloadIndex)) {
                    const QByteArray& frameData = cotpPacket->GetParentFrame()->GetData();
                    if(cotpPayloadIndex > 0 && cotpPayloadIndex < frameData.size()) {
                        uint8_t rdpType = static_cast<uint8_t>(frameData[cotpPayloadIndex]);
                        if(rdpType == 0x02) {
                            // tcpSession->ProtocolFinder.SetConfirmedApplicationLayerProtocol(ApplicationLayerProtocol.SSL, false);
                        }
                    }
                }
            }
        }
        else if (cotpPacket->EncapsulatedProtocol == 1 /* CotpPacket.PayloadProtocol.S7Comm */) {
             if (cotpPacket->GetTpdu() == PacketParser::Packets::CotpPacket::Tpdu::ConnectionRequest) {
                 int partIndex;
                 if(cotpPacket->TryGetVariablePartIndex(partIndex)) {
                     const QByteArray& frameData = cotpPacket->GetParentFrame()->GetData();
                     while (partIndex < cotpPacket->GetPacketEndIndex() && partIndex + 1 < frameData.size()) {
                         uint8_t parameterCode = static_cast<uint8_t>(frameData[partIndex++]);
                         uint8_t parameterLength = static_cast<uint8_t>(frameData[partIndex++]);

                         if (parameterLength > 0 && partIndex + parameterLength <= frameData.size()) {
                             if(parameterCode == static_cast<uint8_t>(PacketParser::Packets::CotpPacket::ParameterCode::SRC_TSAP)) {
                                 bool sTsapIsAscii;
                                 QString sTSAP = GetTsapString(frameData, partIndex, parameterLength, sTsapIsAscii);
                                 if(!sTSAP.isEmpty()) {
                                     parms.insert("Source TSAP", sTSAP);
                                     if(transferIsClientToServer) {
                                         tcpSession->GetClientHost()->AddNumberedExtraDetail("COTP TSAP", sTSAP);
                                     } else {
                                         tcpSession->GetServerHost()->AddNumberedExtraDetail("COTP TSAP", sTSAP);
                                     }
                                 }
                             }
                             if(parameterCode == static_cast<uint8_t>(PacketParser::Packets::CotpPacket::ParameterCode::DST_TSAP)) {
                                 bool dTsapIsAscii;
                                 QString dTSAP = GetTsapString(frameData, partIndex, parameterLength, dTsapIsAscii);
                                 if(!dTSAP.isEmpty()) {
                                     parms.insert("Destination TSAP", dTSAP);
                                     if (transferIsClientToServer) {
                                         tcpSession->GetServerHost()->AddNumberedExtraDetail("COTP TSAP", dTSAP);
                                     } else {
                                         tcpSession->GetClientHost()->AddNumberedExtraDetail("COTP TSAP", dTSAP);
                                     }
                                 }
                             }
                         }
                         partIndex += parameterLength;
                     }
                 }
             }
        }

        if(!parms.isEmpty()) {
            auto pe = std::make_shared<PacketHandlerFramework::Events::ParametersEventArgs>(
                cotpPacket->GetParentFrame(), tcpSession, transferIsClientToServer, parms, details);
            GetMainPacketHandler()->OnParametersDetected(pe);
        }

    }

    return 0;
}

QString CotpPacketHandler::GetTsapString(const QByteArray& data, int index, int length, bool& tsapIsAsciiString) {
    QString tsapString;
    tsapIsAsciiString = PacketParser::Utils::StringManglerUtil::TryGet7BitAsciiString(data, index, length, tsapString);
    if(tsapIsAsciiString) {
        return tsapString;
    } else {
        return PacketParser::Utils::ByteConverter::ToHexString(data, length, index, false, ".");
    }
}

bool CotpPacketHandler::TryGetRdpCookie(const QByteArray& data, int offset, QString& rdpCookie) {
    int index = offset;
    QString line = PacketParser::Utils::ByteConverter::ReadLine(data, index);
    QString RDP_COOKIE_FIELD_HEADER = "Cookie: ";
    if (line.length() > RDP_COOKIE_FIELD_HEADER.length() && line.startsWith(RDP_COOKIE_FIELD_HEADER)) {
        rdpCookie = line.mid(RDP_COOKIE_FIELD_HEADER.length());
        return true;
    }
    return false;
}

void CotpPacketHandler::Reset() {
    cotpDataSegments.clear();
}

}
}
