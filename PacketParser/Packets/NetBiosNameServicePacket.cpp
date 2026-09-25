#include "NetBiosNameServicePacket.hpp"
#include "RawPacket.hpp"
#include "../Utils.hpp"
#include <QRegularExpression>

namespace PacketParser {
namespace Packets {

NetBiosNameServicePacket::ResourceRecord::ResourceRecord(NetBiosNameServicePacket* nbnsPacket, int& offset) {
    name = NetBiosPacket::DecodeNetBiosName(nbnsPacket->ParentFrame(), offset, nbnsPacket);
    
    const uint8_t* frameData = nbnsPacket->ParentFrame()->Data();
    type = Utils::ByteConverter::ToUInt16(frameData, offset);
    offset += 2;
    recordClass = Utils::ByteConverter::ToUInt16(frameData, offset);
    offset += 2;
    ttl = Utils::ByteConverter::ToUInt32(frameData, offset);
    offset += 4;
    uint16_t dataLength = Utils::ByteConverter::ToUInt16(frameData, offset);
    offset += 2;
    data.assign(frameData + offset, frameData + offset + dataLength);
    offset += dataLength;
}

QString NetBiosNameServicePacket::ResourceRecord::NameTrimmed() const {
    QRegularExpression re("(<[\\dA-Z]{2}>$)");
    QString result = name;
    result.remove(re);
    return result;
}

bool NetBiosNameServicePacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& netBiosNameServicePacket) {
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 12 > static_cast<int>(dataLen)) {
        netBiosNameServicePacket = nullptr;
        return false;
    }

    uint16_t questionCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 4);
    uint16_t answerCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 6);
    uint16_t authorityCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 8);
    uint16_t additionalCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 10);

    if (questionCount < 10 && answerCount < 64 && authorityCount < 64 && additionalCount < 64) {
        netBiosNameServicePacket = new NetBiosNameServicePacket(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } else {
        netBiosNameServicePacket = nullptr;
        return false;
    }
}

NetBiosNameServicePacket::NetBiosNameServicePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : NetBiosPacket(parentFrame, packetStartIndex, packetEndIndex, "NetBIOS Name Service")
{
    const uint8_t* data = parentFrame->Data();
    
    transactionID = Utils::ByteConverter::ToUInt16(data, packetStartIndex);
    flags = DnsPacket::HeaderFlags(Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2));
    questionCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 4);
    answerCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 6);
    authorityCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 8);
    additionalCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 10);

    int i = packetStartIndex + 12;
    for (int q = 0; q < questionCount; q++) {
        questionNameDecoded = NetBiosPacket::DecodeNetBiosName(parentFrame, i, this);
        if (i + 4 <= static_cast<int>(parentFrame->DataLength())) {
            questionType = Utils::ByteConverter::ToUInt16(data, i);
            i += 2;
            questionClass = Utils::ByteConverter::ToUInt16(data, i);
            i += 2;
        }
    }

    for (int a = 0; a < answerCount; a++) {
        if (i >= static_cast<int>(parentFrame->DataLength())) break;
        answerResourceRecords.push_back(new ResourceRecord(this, i));
    }
    for (int a = 0; a < authorityCount; a++) {
        if (i >= static_cast<int>(parentFrame->DataLength())) break;
        authorityResourceRecords.push_back(new ResourceRecord(this, i));
    }
    for (int a = 0; a < additionalCount; a++) {
        if (i >= static_cast<int>(parentFrame->DataLength())) break;
        additionalResourceRecords.push_back(new ResourceRecord(this, i));
    }
}

NetBiosNameServicePacket::~NetBiosNameServicePacket() {
    for (auto rr : answerResourceRecords) delete rr;
    for (auto rr : authorityResourceRecords) delete rr;
    for (auto rr : additionalResourceRecords) delete rr;
}

std::vector<AbstractPacket*> NetBiosNameServicePacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);

    if (PacketStartIndex() + 8 < PacketEndIndex()) {
        AbstractPacket* rawPacket = new RawPacket(ParentFrame(), PacketStartIndex() + 8, PacketEndIndex());
        subPackets.push_back(rawPacket);
        std::vector<AbstractPacket*> childSubPackets = rawPacket->GetSubPackets(false);
        subPackets.insert(subPackets.end(), childSubPackets.begin(), childSubPackets.end());
    }
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
