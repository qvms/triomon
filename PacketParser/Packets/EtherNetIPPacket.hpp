#pragma once

#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include <cstdint>
#include <vector>
#include <memory>
#include <QString>

namespace PacketParser {
namespace Packets {

class EtherNetIPPacket : public AbstractPacket, public virtual ISessionPacket {
public:
    enum class EncapsulationCommand : uint16_t {
        NOP = 0x0000,
        //1-3 are reserved
        ListServices = 0x0004,
        ListIdentity = 0x0063,
        ListInterfaces = 0x0064,
        RegisterSession = 0x0065,
        UnRegisterSession = 0x0066,
        SendRRData = 0x006F, //used for CIP
        SendUnitData = 0x0070, //used for CIP
        IndicateStatus = 0x0072,
        Cancel = 0x0073,
    };

    enum class StatusCode : uint32_t {
        Success = 0x0000,
        InvalidCommand = 0x0001,
        InsufficientMemoryResources = 0x0002,
        IncorrectData = 0x0003,
        InvalidSessionHandle = 0x0064,
        InvalidLength = 0x0065,
        UnsupportedProtocolRevision = 0x0069,
    };

    enum class ItemID : uint16_t {
        Address = 0x0000,
        ListIdentityResponse = 0x000C,
        ConnectionBased = 0x00A1,
        ConnectedTransportPacket = 0x00B1,
        UnconnectedMessage = 0x00B2,
        ListServicesResponse = 0x0100,
        SockAddrRequest = 0x8000, //originator-to-target 
        SockAddrResponse = 0x8001, //target-to-originator 
        SequencedAddressItem = 0x8002,
    };

    static const int ENCAPSULATION_HEADER_LENGTH = 24;

    uint16_t Command() const { return _command; }
    uint16_t CommandLength() const { return _commandLength; }
    uint32_t SessionHandle() const { return _sessionHandle; }
    uint32_t Status() const { return _status; }
    uint64_t SenderContext() const { return _senderContext; }

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t sourcePort, uint16_t destinationPort, std::shared_ptr<EtherNetIPPacket>& ethernetIp);

    EtherNetIPPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t sourcePort, uint16_t destinationPort);
    virtual ~EtherNetIPPacket() = default;

    bool PacketHeaderIsComplete() const override;
    int ParsedBytesCount() const override;
    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

protected:
    class CommonPacket : public AbstractPacket {
    public:
        struct DataItem {
            int offset;
            uint16_t typeId;
            uint16_t length;
        };
        
        std::vector<DataItem> DataItems;

        CommonPacket(EtherNetIPPacket* parentEtherNetIPPacket, int packetStartIndex, int packetEndIndex);
        CommonPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
        virtual ~CommonPacket() = default;

        std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

    private:
        EtherNetIPPacket* _parentEtherNetIPPacket;
        std::vector<std::unique_ptr<AbstractPacket>> _subPackets;
    };

private:
    uint16_t _command;
    uint16_t _commandLength;
    uint32_t _sessionHandle;
    uint32_t _status;
    uint64_t _senderContext;
    
    std::vector<std::unique_ptr<AbstractPacket>> _subPackets;
};

} }
