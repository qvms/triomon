#include "HpSwitchProtocolPacketHandler.hpp"
#include "../../PacketHandlerFramework/MainPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

HpSwitchProtocolPacketHandler::HpSwitchProtocolPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

void HpSwitchProtocolPacketHandler::ExtractDataFromPacket(void* transportPacket) {
    // Handled by IPacketHandler interface simulation
}

void HpSwitchProtocolPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    for (const auto& p : packetList) {
        if (auto hpPacket = std::dynamic_pointer_cast<PacketParser::Packets::HpSwitchProtocolPacket>(p)) {
            for (const auto& f : packetList) {
                if (auto field = std::dynamic_pointer_cast<PacketParser::Packets::HpSwitchProtocolPacket::HpSwField>(f)) {
                    if (field->TypeByte() == static_cast<uint8_t>(PacketParser::Packets::HpSwitchProtocolPacket::HpSwField::FieldType::IpAddress)) {
                        const std::vector<uint8_t>& valBytes = field->ValueBytes();
                        if(valBytes.size() == 4) {
                            uint32_t ipVal = (static_cast<uint8_t>(valBytes[0]) << 24) | (static_cast<uint8_t>(valBytes[1]) << 16) | (static_cast<uint8_t>(valBytes[2]) << 8) | static_cast<uint8_t>(valBytes[3]);
                            QHostAddress ip(ipVal);

                            if (!sourceHost || sourceHost->IPAddress() != ip) {
                                if (GetMainPacketHandler()->GetNetworkHostList()->ContainsIP(ip)) {
                                    sourceHost = GetMainPacketHandler()->GetNetworkHostList()->GetNetworkHost(ip);
                                } else {
                                    sourceHost = std::make_shared<PacketParser::NetworkHost>(ip);
                                    GetMainPacketHandler()->GetNetworkHostList()->Add(sourceHost);
                                }
                            }
                        }
                    }
                }
            }

            if (sourceHost) {
                for (const auto& f : packetList) {
                    if (auto field = std::dynamic_pointer_cast<PacketParser::Packets::HpSwitchProtocolPacket::HpSwField>(f)) {
                        ExtractDataInternal(sourceHost, field);
                    }
                }
            }
        }
    }
}

void HpSwitchProtocolPacketHandler::ExtractDataInternal(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::Packets::HpSwitchProtocolPacket::HpSwField> hpswField) {
    if (hpswField->TypeByte() == static_cast<uint8_t>(PacketParser::Packets::HpSwitchProtocolPacket::HpSwField::FieldType::DeviceName)) {
        if (!sourceHost->ExtraDetailsList().contains("HPSW Device Name")) {
            sourceHost->ExtraDetailsList().insert("HPSW Device Name", hpswField->ValueString());
            sourceHost->AddHostName(hpswField->ValueString(), hpswField->GetPacketTypeDescription());
        }
    }
    else if (hpswField->TypeByte() == static_cast<uint8_t>(PacketParser::Packets::HpSwitchProtocolPacket::HpSwField::FieldType::Version)) {
        if (!sourceHost->ExtraDetailsList().contains("HPSW Firmware version")) {
            sourceHost->ExtraDetailsList().insert("HPSW Firmware version", hpswField->ValueString());
        }
    }
    else if (hpswField->TypeByte() == static_cast<uint8_t>(PacketParser::Packets::HpSwitchProtocolPacket::HpSwField::FieldType::Config)) {
        if (!sourceHost->ExtraDetailsList().contains("HPSW Config")) {
            sourceHost->ExtraDetailsList().insert("HPSW Config", hpswField->ValueString());
        }
    }
    else if (hpswField->TypeByte() == static_cast<uint8_t>(PacketParser::Packets::HpSwitchProtocolPacket::HpSwField::FieldType::MacAddress)) {
        sourceHost->MacAddress() = hpswField->ValueBytes();
    }
}

void HpSwitchProtocolPacketHandler::Reset() {
    // throw new Exception("The method or operation is not implemented.");
}

}
}
