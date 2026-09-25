#include "EtherNetIPPacket.hpp"
#include "../../Utils/ByteConverter.hpp"
#include "../Frame.hpp"
#include "CipPacket.hpp"
#include <iostream>
#include <stdexcept>

namespace PacketParser {
namespace Packets {

bool EtherNetIPPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t sourcePort, uint16_t destinationPort, std::shared_ptr<EtherNetIPPacket>& ethernetIp) {
    ethernetIp = nullptr;
    int ethernetIpDataLength = packetEndIndex - packetStartIndex + 1;
    //min length is 24 bytes (encapsulation header)
    if (ethernetIpDataLength < ENCAPSULATION_HEADER_LENGTH)
        return false;
    
    uint16_t commandLength = Utils::ByteConverter::ToUInt16(parentFrame->Data(), packetStartIndex + 2, true);
    if (ENCAPSULATION_HEADER_LENGTH + commandLength > ethernetIpDataLength)
        return false;
    
    try {
        ethernetIp = std::make_shared<EtherNetIPPacket>(parentFrame, packetStartIndex, packetEndIndex, sourcePort, destinationPort);
        return true;
    }
    catch (const std::exception& ex) {
        const_cast<Frame*>(parentFrame)->addError(Frame::Error(QString("Error parsing EtherNet/IP packet in frame ") + QString::number(parentFrame->FrameNumber()) + ":" + ex.what()));
        return false;
    }
    catch (...) {
        const_cast<Frame*>(parentFrame)->addError(Frame::Error(QString("Error parsing EtherNet/IP packet in frame ") + QString::number(parentFrame->FrameNumber()) + ": Unknown exception"));
        return false;
    }
}

EtherNetIPPacket::EtherNetIPPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t sourcePort, uint16_t destinationPort)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "EtherNet/IP")
{
    (void)sourcePort;
    (void)destinationPort;
    const QByteArray& data = parentFrame->Data();
    _command = Utils::ByteConverter::ToUInt16(data, packetStartIndex, true);
    _commandLength = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2, true);
    _sessionHandle = Utils::ByteConverter::ToUInt32(data, packetStartIndex + 4, 4, true);
    _status = Utils::ByteConverter::ToUInt32(data, packetStartIndex + 8, 4, true);
    _senderContext = Utils::ByteConverter::ToUInt64(data, packetStartIndex + 12, true);
    
    // uint optionsFlags = Utils.ByteConverter.ToUInt32(parentFrame.Data, packetStartIndex + 20, 4, true);
}

bool EtherNetIPPacket::PacketHeaderIsComplete() const {
    return ParsedBytesCount() >= ENCAPSULATION_HEADER_LENGTH;
}

int EtherNetIPPacket::ParsedBytesCount() const {
    if (PacketLength() >= ENCAPSULATION_HEADER_LENGTH + _commandLength)
        return ENCAPSULATION_HEADER_LENGTH + _commandLength;
    else
        return 0;
}

std::vector<AbstractPacket*> EtherNetIPPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> packets;
    if (includeSelfReference)
        packets.push_back(this);

    if (_subPackets.empty()) {
        CommonPacket* packet = nullptr;
        if (_command == static_cast<uint16_t>(EncapsulationCommand::SendRRData)) {
            packet = new CommonPacket(this, PacketStartIndex() + ENCAPSULATION_HEADER_LENGTH + 6, PacketStartIndex() + ENCAPSULATION_HEADER_LENGTH + _commandLength);
        }
        else if (_command == static_cast<uint16_t>(EncapsulationCommand::SendUnitData)) {
            packet = new CommonPacket(this, PacketStartIndex() + ENCAPSULATION_HEADER_LENGTH + 6, PacketStartIndex() + ENCAPSULATION_HEADER_LENGTH + _commandLength);
        }

        if (packet != nullptr) {
            _subPackets.push_back(std::unique_ptr<AbstractPacket>(packet));
        }
    }

    for (const auto& sub : _subPackets) {
        packets.push_back(sub.get());
        auto subSub = sub->GetSubPackets(false);
        packets.insert(packets.end(), subSub.begin(), subSub.end());
    }

    return packets;
}

EtherNetIPPacket::CommonPacket::CommonPacket(EtherNetIPPacket* parentEtherNetIPPacket, int packetStartIndex, int packetEndIndex)
    : CommonPacket(parentEtherNetIPPacket->ParentFrame(), packetStartIndex, packetEndIndex)
{
    _parentEtherNetIPPacket = parentEtherNetIPPacket;
}

EtherNetIPPacket::CommonPacket::CommonPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "Ethernet/IP Common Packet"), _parentEtherNetIPPacket(nullptr)
{
    const QByteArray& data = parentFrame->Data();
    if (packetStartIndex + 2 <= data.size()) {
        uint16_t itemCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex, true);
        DataItems.resize(itemCount);

        int itemOffset = PacketStartIndex() + 2;
        for (int i = 0; i < itemCount; i++) {
            if (itemOffset + 4 > data.size()) break;
            uint16_t itemTypeId = Utils::ByteConverter::ToUInt16(data, itemOffset, true);
            uint16_t itemLength = Utils::ByteConverter::ToUInt16(data, itemOffset + 2, true);
            
            DataItem item;
            item.offset = itemOffset + 4;
            item.typeId = itemTypeId;
            item.length = itemLength;
            DataItems[i] = item;
            
            itemOffset += 4 + itemLength;
        }
    }
}

std::vector<AbstractPacket*> EtherNetIPPacket::CommonPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> packets;
    if (includeSelfReference)
        packets.push_back(this);

    if (_subPackets.empty()) {
        for(const auto& item : DataItems) {
            if (item.typeId == static_cast<uint16_t>(ItemID::UnconnectedMessage)) {
                auto cip = new CipPacket(ParentFrame(), item.offset, item.offset + item.length - 1);
                _subPackets.push_back(std::unique_ptr<AbstractPacket>(cip));
            }
            else if (item.typeId == static_cast<uint16_t>(ItemID::ConnectedTransportPacket)) {
                if (item.offset + 2 <= ParentFrame()->Data().size()) {
                    uint16_t cipSequenceCount = Utils::ByteConverter::ToUInt16(ParentFrame()->Data(), item.offset, true);
                    int startIndex = item.offset + 2;
                    if(cipSequenceCount > 0) { //only get the first one
                        auto cip = new CipPacket(ParentFrame(), startIndex, item.offset + item.length - 1);
                        _subPackets.push_back(std::unique_ptr<AbstractPacket>(cip));
                    }
                }
            }
        }
    }

    for (const auto& sub : _subPackets) {
        packets.push_back(sub.get());
        auto embedded = sub->GetSubPackets(false);
        packets.insert(packets.end(), embedded.begin(), embedded.end());
    }

    return packets;
}

} }
