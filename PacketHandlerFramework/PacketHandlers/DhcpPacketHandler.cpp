#include "DhcpPacketHandler.hpp"
#include "../../PacketHandlerFramework/Events/ParametersEventArgs.hpp"
#include "../../PacketHandlerFramework/MainPacketHandler.hpp"
#include "../../PacketParser/Utils/ByteConverter.hpp"
#include "../../PacketParser/Packets/UdpPacket.hpp"
#include <QVariantMap>
#include <QRegularExpression>

namespace PacketHandlerFramework {
namespace PacketHandlers {

DhcpPacketHandler::DhcpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

void DhcpPacketHandler::ExtractDataFromPacket(void* transportPacket) {
    // Handled by IPacketHandler interface simulation
}

void DhcpPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    for(auto& p : packetList) {
        auto dhcpPacket = std::dynamic_pointer_cast<PacketParser::Packets::DhcpPacket>(p);
        if(dhcpPacket) {
            ExtractDataInternal(sourceHost, destinationHost, dhcpPacket);
        }
    }
}

void DhcpPacketHandler::Reset() {
    previousIpList.clear();
}

void DhcpPacketHandler::ExtractDataInternal(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, std::shared_ptr<PacketParser::Packets::DhcpPacket> dhcpPacket) {
    if (dhcpPacket->OpCode() == PacketParser::Packets::DhcpPacket::OpCodeValue::BootRequest && (sourceHost->MacAddress().empty() || dhcpPacket->ClientMacAddress() != sourceHost->MacAddress())) {
        sourceHost->MacAddress() = dhcpPacket->ClientMacAddress();
    }
    else if (dhcpPacket->OpCode() == PacketParser::Packets::DhcpPacket::OpCodeValue::BootReply && (destinationHost->MacAddress().empty() || dhcpPacket->ClientMacAddress() != destinationHost->MacAddress())) {
        destinationHost->MacAddress() = dhcpPacket->ClientMacAddress();
    }

    if (dhcpPacket->OpCode() == PacketParser::Packets::DhcpPacket::OpCodeValue::BootReply && !dhcpPacket->GatewayIpAddress().isNull() && dhcpPacket->GatewayIpAddress() != QHostAddress::Any) {
        // Assume thread-safety handling if required, or simple map insertion
        if (!destinationHost->ExtraDetailsList().contains("Default Gateway")) {
            destinationHost->ExtraDetailsList()["Default Gateway"] = dhcpPacket->GatewayIpAddress().toString();
        }
    }

    QVariantMap optionParameterList;

    for (const auto& option : dhcpPacket->OptionList()) {
        if (option.OptionCode() == 12) { // hostname
            QByteArray ba(reinterpret_cast<const char*>(option.OptionValue().data()), option.OptionValue().size());
            QString hostname = PacketParser::Utils::ByteConverter::ReadString(ba);
            sourceHost->AddHostName(hostname, dhcpPacket->GetPacketTypeDescription());
            optionParameterList.insert("DHCP Option 12 Hostname", hostname);
        }
        else if (option.OptionCode() == 15) { // Domain Name
            QByteArray ba(reinterpret_cast<const char*>(option.OptionValue().data()), option.OptionValue().size());
            QString domain = PacketParser::Utils::ByteConverter::ReadString(ba);
            sourceHost->AddDomainName(domain);
            optionParameterList.insert("DHCP Option 15 Domain", domain);
        }
        else if (option.OptionCode() == 50) { // requested IP address
            if (dhcpPacket->DhcpMessageType() == 3) { // Must be a DHCP Request
                QByteArray ba(reinterpret_cast<const char*>(option.OptionValue().data()), option.OptionValue().size());
                // Handle 4-byte IPv4 properly... Assuming 4 bytes
                uint32_t ipVal = 0;
                if(ba.size() == 4) {
                    ipVal = (static_cast<uint8_t>(ba[0]) << 24) | (static_cast<uint8_t>(ba[1]) << 16) | (static_cast<uint8_t>(ba[2]) << 8) | static_cast<uint8_t>(ba[3]);
                }
                QHostAddress requestedIpAddress(ipVal);

                if (sourceHost->IPAddress() != requestedIpAddress) {
                    if (!GetMainPacketHandler()->GetNetworkHostList()->ContainsIP(requestedIpAddress)) {
                        auto clonedHost = std::make_shared<PacketParser::NetworkHost>(requestedIpAddress);
                        clonedHost->MacAddress() = sourceHost->MacAddress();
                        GetMainPacketHandler()->GetNetworkHostList()->Add(clonedHost);
                        sourceHost = clonedHost;
                    } else {
                        sourceHost = GetMainPacketHandler()->GetNetworkHostList()->GetNetworkHost(requestedIpAddress);
                        if (dhcpPacket->OpCode() == PacketParser::Packets::DhcpPacket::OpCodeValue::BootRequest && (sourceHost->MacAddress().empty() || dhcpPacket->ClientMacAddress() != sourceHost->MacAddress())) {
                            sourceHost->MacAddress() = dhcpPacket->ClientMacAddress();
                        }
                    }
                }
                QString macStr = PacketParser::Utils::ByteConverter::ToHexString(sourceHost->MacAddress(), sourceHost->MacAddress().size(), 0, false, ":");
                if (!sourceHost->MacAddress().empty() && previousIpList.contains(macStr)) {
                    sourceHost->AddNumberedExtraDetail("Previous IP", previousIpList[macStr].toString());
                    previousIpList.remove(macStr);
                }
            } else if (dhcpPacket->DhcpMessageType() == 1) { // DHCP discover
                QByteArray ba(reinterpret_cast<const char*>(option.OptionValue().data()), option.OptionValue().size());
                uint32_t ipVal = 0;
                if(ba.size() == 4) {
                    ipVal = (static_cast<uint8_t>(ba[0]) << 24) | (static_cast<uint8_t>(ba[1]) << 16) | (static_cast<uint8_t>(ba[2]) << 8) | static_cast<uint8_t>(ba[3]);
                }
                QHostAddress requestedIpAddress(ipVal);
                QString macStr = PacketParser::Utils::ByteConverter::ToHexString(sourceHost->MacAddress(), sourceHost->MacAddress().size(), 0, false, ":");
                previousIpList[macStr] = requestedIpAddress;
            }
        }
        else if (option.OptionCode() == 60) { // vendor class identifier
            QByteArray ba(reinterpret_cast<const char*>(option.OptionValue().data()), option.OptionValue().size());
            QString vendorCode = PacketParser::Utils::ByteConverter::ReadString(ba);
            sourceHost->AddDhcpVendorCode(vendorCode);
            optionParameterList.insert("DHCP Option 60 Vendor Code", vendorCode);
        }
        else if (option.OptionCode() == 81) { // Client Fully Qualified Domain Name
            QByteArray ba(reinterpret_cast<const char*>(option.OptionValue().data()), option.OptionValue().size());
            QString domain = PacketParser::Utils::ByteConverter::ReadString(ba, 3, ba.size() - 3);
            sourceHost->AddHostName(domain, dhcpPacket->GetPacketTypeDescription());
            optionParameterList.insert("DHCP Option 81 Domain", domain);
        }
        else if (option.OptionCode() == 125) { // V-I Vendor-specific Information
            QByteArray ba(reinterpret_cast<const char*>(option.OptionValue().data()), option.OptionValue().size());
            uint32_t enterpriceNumber = PacketParser::Utils::ByteConverter::ToUInt32(ba, 0);
            optionParameterList.insert("DHCP Option 125 Enterprise Number", QString::number(enterpriceNumber));
            if(ba.size() > 4) {
                uint8_t dataLen = static_cast<uint8_t>(ba[4]);
                if (dataLen > 0 && ba.size() >= 5 + dataLen) {
                    QString optionData = PacketParser::Utils::ByteConverter::ReadString(ba, 5, dataLen);
                    optionParameterList.insert("DHCP Option 125 Data", optionData);
                }
            }
        } else {
            QByteArray ba(reinterpret_cast<const char*>(option.OptionValue().data()), option.OptionValue().size());
            QString optionValueString = PacketParser::Utils::ByteConverter::ReadString(ba);
            QRegularExpression re("[^\\x20-\\x7E]");
            if (!re.match(optionValueString).hasMatch()) {
                optionParameterList.insert("DHCP Option " + QString::number(option.OptionCode()), optionValueString);
            }
        }
    }

    if (!optionParameterList.isEmpty()) {
        std::shared_ptr<PacketParser::Packets::UdpPacket> udpPacket = nullptr;
        for (auto& p : dhcpPacket->GetParentFrame()->GetPacketList()) {
            udpPacket = std::dynamic_pointer_cast<PacketParser::Packets::UdpPacket>(p);
            if (udpPacket) {
                break;
            }
        }
        if (udpPacket) {
            auto ea = std::make_shared<PacketHandlerFramework::Events::ParametersEventArgs>(
                dhcpPacket->GetParentFrame()->GetFrameNumber(), sourceHost, destinationHost,
                udpPacket->TransportProtocol(), udpPacket->SourcePort(), udpPacket->DestinationPort(),
                optionParameterList, dhcpPacket->GetParentFrame()->GetTimestamp(), "DHCP Option");
            GetMainPacketHandler()->OnParametersDetected(ea);
        }
    }
}

}
}
