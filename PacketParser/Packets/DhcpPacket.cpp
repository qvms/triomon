#include "DhcpPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

DhcpPacket::Option::Option(const uint8_t* frameData, int optionStartIndex) {
    optionCode = frameData[optionStartIndex];
    dataLength = frameData[optionStartIndex + 1];
    value.assign(frameData + optionStartIndex + 2, frameData + optionStartIndex + 2 + dataLength);
}

DhcpPacket::DhcpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "DHCP (Bootstrap protocol)")
    , dhcpMessageType(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 240 > static_cast<int>(dataLen)) return;

    opCode = static_cast<OpCodeValue>(data[packetStartIndex]);
    transactionID = Utils::ByteConverter::ToUInt32(data, packetStartIndex + 4);
    secondsElapsed = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 8);

    uint32_t cIp = (data[packetStartIndex + 12] << 24) | (data[packetStartIndex + 13] << 16) | (data[packetStartIndex + 14] << 8) | data[packetStartIndex + 15];
    clientIpAddress = QHostAddress(cIp);

    uint32_t yIp = (data[packetStartIndex + 16] << 24) | (data[packetStartIndex + 17] << 16) | (data[packetStartIndex + 18] << 8) | data[packetStartIndex + 19];
    yourIpAddress = QHostAddress(yIp);

    uint32_t sIp = (data[packetStartIndex + 20] << 24) | (data[packetStartIndex + 21] << 16) | (data[packetStartIndex + 22] << 8) | data[packetStartIndex + 23];
    serverIpAddress = QHostAddress(sIp);

    uint32_t gIp = (data[packetStartIndex + 24] << 24) | (data[packetStartIndex + 25] << 16) | (data[packetStartIndex + 26] << 8) | data[packetStartIndex + 27];
    gatewayIpAddress = QHostAddress(gIp);

    clientMacAddress.assign(data + packetStartIndex + 28, data + packetStartIndex + 34);

    if (!ParentFrame()->QuickParse()) {
        addAttribute("OpCode", QString::number(static_cast<uint8_t>(opCode)));
        addAttribute("Transaction ID", QString("0x%1").arg(transactionID, 8, 16, QChar('0')).toUpper());
        addAttribute("Seconds elapsed", QString::number(secondsElapsed));
        addAttribute("Client IP Address", clientIpAddress.toString());
        addAttribute("Your IP Address", yourIpAddress.toString());
        addAttribute("Server IP Address", serverIpAddress.toString());
        addAttribute("Gateway IP Address", gatewayIpAddress.toString());
        
        QString macStr;
        for (size_t i = 0; i < clientMacAddress.size(); i++) {
            if (i > 0) macStr += "-";
            macStr += QString("%1").arg(clientMacAddress[i], 2, 16, QChar('0')).toUpper();
        }
        addAttribute("Client MAC Address", macStr);
    }

    int index = packetStartIndex + 240;
    while (index < packetEndIndex && index + 1 < static_cast<int>(dataLen)) {
        Option option(data, index);
        if (option.OptionCode() == 0xff) break; // End Option

        if (option.OptionCode() == 3 && option.OptionValue().size() == 4) { // Default Gateway
            uint32_t optGIp = (option.OptionValue()[0] << 24) | (option.OptionValue()[1] << 16) | (option.OptionValue()[2] << 8) | option.OptionValue()[3];
            gatewayIpAddress = QHostAddress(optGIp);
        } else if (option.OptionCode() == 53 && option.OptionValue().size() == 1) { // DHCP Message Type
            dhcpMessageType = option.OptionValue()[0];
        }

        optionList.push_back(option);
        if (!ParentFrame()->QuickParse()) {
            addAttribute("DHCP Options", QString::number(option.OptionCode()));
        }

        index += option.OptionValue().size() + 2;
    }
}

std::vector<AbstractPacket*> DhcpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
