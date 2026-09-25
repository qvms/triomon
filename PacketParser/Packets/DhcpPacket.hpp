#pragma once
#include "AbstractPacket.hpp"
#include <QHostAddress>
#include <vector>

namespace PacketParser {
namespace Packets {

class DhcpPacket : public AbstractPacket {
public:
    enum class OpCodeValue : uint8_t { BootRequest = 0x01, BootReply = 0x02 };

    class Option {
    private:
        uint8_t optionCode;
        uint8_t dataLength;
        std::vector<uint8_t> value;

    public:
        Option(const uint8_t* frameData, int optionStartIndex);
        uint8_t OptionCode() const { return optionCode; }
        const std::vector<uint8_t>& OptionValue() const { return value; }
    };

private:
    OpCodeValue opCode;
    uint32_t transactionID;
    uint16_t secondsElapsed;
    QHostAddress clientIpAddress;
    QHostAddress yourIpAddress;
    QHostAddress serverIpAddress;
    QHostAddress gatewayIpAddress;
    std::vector<uint8_t> clientMacAddress;
    std::vector<Option> optionList;
    uint8_t dhcpMessageType; // 1=Discover, 2=Offer, 3=Request, 5=Ack, 8=Inform

public:
    DhcpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    OpCodeValue OpCode() const { return opCode; }
    uint32_t TransactionID() const { return transactionID; }
    uint16_t SecondsElapsed() const { return secondsElapsed; }
    QHostAddress ClientIpAddress() const { return clientIpAddress; }
    QHostAddress YourIpAddress() const { return yourIpAddress; }
    QHostAddress ServerIpAddress() const { return serverIpAddress; }
    QHostAddress GatewayIpAddress() const { return gatewayIpAddress; }
    std::vector<uint8_t> ClientMacAddress() const { return clientMacAddress; }
    const std::vector<Option>& OptionList() const { return optionList; }
    uint8_t DhcpMessageType() const { return dhcpMessageType; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
