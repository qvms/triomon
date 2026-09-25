#include "SnmpPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

SnmpPacketHandler::SnmpPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> SnmpPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::SnmpPacket>() };
}

ApplicationLayerProtocol SnmpPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::Snmp;
}

void SnmpPacketHandler::ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::UdpPacket> udpPacket;
    std::shared_ptr<PacketParser::Packets::SnmpPacket> snmpPacket;
    
    for (const auto& p : packetList) {
        if (auto udp = std::dynamic_pointer_cast<PacketParser::Packets::UdpPacket>(p)) {
            udpPacket = udp;
        } else if (auto snmp = std::dynamic_pointer_cast<PacketParser::Packets::SnmpPacket>(p)) {
            snmpPacket = snmp;
        }
    }
    
    if (udpPacket && snmpPacket) {
        if (!snmpPacket->Community().isEmpty()) {
            MainPacketHandler()->AddCredential(std::make_shared<NetworkCredential>(
                sourceHost, destinationHost, "SNMP Community", "", snmpPacket->Community(), snmpPacket->ParentFrame()->GetTimestamp()
            ));
        }
        
        if (snmpPacket->Type() == PacketParser::Packets::SnmpPacket::GetRequest || 
            snmpPacket->Type() == PacketParser::Packets::SnmpPacket::GetNextRequest || 
            snmpPacket->Type() == PacketParser::Packets::SnmpPacket::GetResponse || 
            snmpPacket->Type() == PacketParser::Packets::SnmpPacket::SetRequest) {
            
            QString packetDescription = "";
            if (snmpPacket->Type() == PacketParser::Packets::SnmpPacket::GetRequest) packetDescription = "SNMP GetRequest";
            else if (snmpPacket->Type() == PacketParser::Packets::SnmpPacket::GetNextRequest) packetDescription = "SNMP GetNextRequest";
            else if (snmpPacket->Type() == PacketParser::Packets::SnmpPacket::GetResponse) packetDescription = "SNMP GetResponse";
            else if (snmpPacket->Type() == PacketParser::Packets::SnmpPacket::SetRequest) packetDescription = "SNMP SetRequest";
            
            QMap<QString, QString> tmpCol;
            for (const auto& parameter : snmpPacket->Parameters()) {
                if (parameter.ValueType == 0x06) {
                    QString valString = parameter.ValueAsString();
                    if (!valString.isEmpty()) {
                        tmpCol.insert(parameter.Name, parameter.ValueAsString());
                    }
                } else if (parameter.ValueType == 0x40 || parameter.ValueType == 0x02 || parameter.ValueType == 0x43) {
                    tmpCol.insert(parameter.Name, parameter.ValueAsString());
                } else if (parameter.ValueType == 0x04 && parameter.ValueLength() > 0) {
                    QString snmpString = parameter.ValueAsString();
                    if (snmpString.length() > 0 && 
                        snmpString[0].isPrint() && snmpString[0].isLetterOrNumber()) {
                        tmpCol.insert(parameter.Name, snmpString);
                    } else if (parameter.ValueLength() == 6) {
                        QByteArray valBytes = parameter.Value;
                        QString macString = QString("%1:%2:%3:%4:%5:%6")
                            .arg(quint8(valBytes[0]), 2, 16, QChar('0'))
                            .arg(quint8(valBytes[1]), 2, 16, QChar('0'))
                            .arg(quint8(valBytes[2]), 2, 16, QChar('0'))
                            .arg(quint8(valBytes[3]), 2, 16, QChar('0'))
                            .arg(quint8(valBytes[4]), 2, 16, QChar('0'))
                            .arg(quint8(valBytes[5]), 2, 16, QChar('0')).toUpper();
                        tmpCol.insert(parameter.Name, macString);
                    }
                    
                    if (parameter.Name == "1.3.6.1.2.1.1.1.0" || parameter.Name == "1.3.6.1.2.1.1.5.0") {
                        if (snmpPacket->Type() == PacketParser::Packets::SnmpPacket::GetResponse) {
                            sourceHost->AddHostName(snmpString, packetDescription);
                        } else if (snmpPacket->Type() == PacketParser::Packets::SnmpPacket::SetRequest) {
                            destinationHost->AddHostName(snmpString, packetDescription);
                        }
                    } else if (snmpString.contains("Windows", Qt::CaseInsensitive)) {
                        sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SnmpParameter, snmpString);
                    } else if (snmpString.contains("Linux", Qt::CaseInsensitive)) {
                        sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SnmpParameter, snmpString);
                    } else if (snmpString.contains("Darwin", Qt::CaseInsensitive)) {
                        sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SnmpParameter, snmpString);
                    } else if (snmpString.contains("Cisco", Qt::CaseInsensitive)) {
                        sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SnmpParameter, snmpString);
                    } else if (snmpString.contains("HP", Qt::CaseInsensitive)) {
                        sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SnmpParameter, snmpString);
                    } else if (snmpString.contains("LaserJet", Qt::CaseInsensitive)) {
                        sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SnmpParameter, snmpString);
                    } else if (snmpString.contains("JETDIRECT", Qt::CaseInsensitive)) {
                        sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SnmpParameter, snmpString);
                    } else if (snmpString.contains("http", Qt::CaseInsensitive)) {
                        sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SnmpParameter, snmpString);
                    } else if (snmpString.contains("Firmware", Qt::CaseInsensitive)) {
                        sourceHost->AddNumberedExtraDetail(NetworkHost::ExtraDetailType::SnmpParameter, snmpString);
                    }
                }
            }
            if (tmpCol.size() > 0) {
                MainPacketHandler()->OnParametersDetected(Events::ParametersEventArgs(
                    snmpPacket->ParentFrame()->GetFrameNumber(),
                    sourceHost, destinationHost, udpPacket->TransportProtocol(),
                    udpPacket->SourcePort(), udpPacket->DestinationPort(),
                    tmpCol, snmpPacket->ParentFrame()->GetTimestamp(), packetDescription
                ));
            }
        }
    }
}

void SnmpPacketHandler::Reset() {
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
