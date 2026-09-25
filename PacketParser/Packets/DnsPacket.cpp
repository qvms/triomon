#include "DnsPacket.hpp"
#include "../Utils.hpp"
#include <stdexcept>
#include <QStringList>

namespace PacketParser {
namespace Packets {

DnsPacket::NameLabel::NameLabel(const uint8_t* sourceData, int labelStartPosition)
    : labelStartPosition(labelStartPosition)
{
    labelByteCount = sourceData[labelStartPosition];
    if (labelByteCount > 63) {
        // Technically handle pointer... but simplified for now per original code constraints
        labelByteCount = 0;
    } else {
        for (int i = 0; i < labelByteCount; i++) {
            decodedName.append(static_cast<char>(sourceData[labelStartPosition + 1 + i]));
        }
    }
}

std::vector<DnsPacket::NameLabel> DnsPacket::GetNameLabelList(const uint8_t* sourceData, int baseOffset, int relativeStartIndex) {
    std::vector<NameLabel> list;
    int currentIndex = baseOffset + relativeStartIndex;
    while (sourceData[currentIndex] != 0x00) {
        if ((sourceData[currentIndex] & 0xC0) == 0xC0) {
            uint16_t offset = Utils::ByteConverter::ToUInt16(sourceData, currentIndex) & 0x3FFF;
            std::vector<NameLabel> pointerLabels = GetNameLabelList(sourceData, baseOffset, offset);
            list.insert(list.end(), pointerLabels.begin(), pointerLabels.end());
            break;
        } else {
            NameLabel label(sourceData, currentIndex);
            list.push_back(label);
            currentIndex += label.LabelByteCount() + 1;
        }
    }
    return list;
}

DnsPacket::ResourceRecord::ResourceRecord(DnsPacket* parentPacket, int startIndex)
    : parentPacket(parentPacket)
{
    const uint8_t* data = parentPacket->ParentFrame()->Data();
    std::vector<NameLabel> nameLabelList = DnsPacket::GetNameLabelList(data, parentPacket->PacketStartIndex(), startIndex - parentPacket->PacketStartIndex());

    for (const auto& label : nameLabelList) {
        answerRequestedNameDecoded << label.ToString();
    }

    // Rough approximation of ByteCount calculation skipping the label bytes
    int currentIndex = startIndex;
    while (data[currentIndex] != 0 && (data[currentIndex] & 0xC0) != 0xC0) {
        currentIndex += data[currentIndex] + 1;
    }
    if ((data[currentIndex] & 0xC0) == 0xC0) {
        currentIndex += 2;
    } else {
        currentIndex += 1;
    }

    answerType = Utils::ByteConverter::ToUInt16(data, currentIndex);
    answerClass = Utils::ByteConverter::ToUInt16(data, currentIndex + 2);
    answerTimeToLive = Utils::ByteConverter::ToUInt32(data, currentIndex + 4);
    answerDataLength = Utils::ByteConverter::ToUInt16(data, currentIndex + 8);
    recordByteCount = (currentIndex + 10 - startIndex) + answerDataLength;

    if (parentPacket->Flags().OperationCode() == static_cast<uint8_t>(HeaderFlags::OperationCodes::Query) && answerType != static_cast<uint16_t>(RRTypes::CNAME)) {
        for (int i = 0; i < answerDataLength; i++) {
            answerRepliedNameDecoded << QString::number(data[currentIndex + 10 + i]);
        }
    } else if (parentPacket->Flags().OperationCode() == static_cast<uint8_t>(HeaderFlags::OperationCodes::Query) && answerType == static_cast<uint16_t>(RRTypes::CNAME)) {
        std::vector<NameLabel> answerRepliedName = DnsPacket::GetNameLabelList(data, parentPacket->PacketStartIndex(), currentIndex + 10 - parentPacket->PacketStartIndex());
        for (const auto& label : answerRepliedName) {
            answerRepliedNameDecoded << label.ToString();
        }
    } else if (parentPacket->Flags().OperationCode() == static_cast<uint8_t>(HeaderFlags::OperationCodes::InverseQuery)) {
        std::vector<NameLabel> nameLabelList2 = DnsPacket::GetNameLabelList(data, parentPacket->PacketStartIndex(), currentIndex + 10 - parentPacket->PacketStartIndex());
        for (const auto& label : nameLabelList2) {
            answerRepliedNameDecoded << label.ToString();
        }
    }
}

QHostAddress DnsPacket::ResourceRecord::IP() const {
    if (parentPacket->Flags().OperationCode() == static_cast<uint8_t>(HeaderFlags::OperationCodes::Query) && answerType == static_cast<uint16_t>(RRTypes::HostAddress)) {
        if (answerRepliedNameDecoded.size() >= 4) {
            uint32_t ip = (answerRepliedNameDecoded[0].toUInt() << 24) |
                          (answerRepliedNameDecoded[1].toUInt() << 16) |
                          (answerRepliedNameDecoded[2].toUInt() << 8) |
                          (answerRepliedNameDecoded[3].toUInt());
            return QHostAddress(ip);
        }
    } else if (parentPacket->Flags().OperationCode() == static_cast<uint8_t>(HeaderFlags::OperationCodes::InverseQuery)) {
        if (answerRequestedNameDecoded.size() >= 4) {
            uint32_t ip = (answerRequestedNameDecoded[0].toUInt() << 24) |
                          (answerRequestedNameDecoded[1].toUInt() << 16) |
                          (answerRequestedNameDecoded[2].toUInt() << 8) |
                          (answerRequestedNameDecoded[3].toUInt());
            return QHostAddress(ip);
        }
    }
    return QHostAddress();
}

QString DnsPacket::ResourceRecord::PrimaryName() const {
    if (answerType == static_cast<uint16_t>(RRTypes::CNAME) && !answerRepliedNameDecoded.empty()) {
        return answerRepliedNameDecoded.join(".");
    }
    return QString();
}

QString DnsPacket::ResourceRecord::DNS() const {
    if (parentPacket->Flags().OperationCode() == static_cast<uint8_t>(HeaderFlags::OperationCodes::Query)) {
        if (!answerRequestedNameDecoded.empty()) return answerRequestedNameDecoded.join(".");
    } else if (parentPacket->Flags().OperationCode() == static_cast<uint8_t>(HeaderFlags::OperationCodes::InverseQuery)) {
        if (!answerRepliedNameDecoded.empty()) return answerRepliedNameDecoded.join(".");
    }
    return QString();
}

DnsPacket::DnsPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "DNS")
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 12 > static_cast<int>(dataLen)) return;

    transactionID = Utils::ByteConverter::ToUInt16(data, packetStartIndex);
    headerFlags = HeaderFlags(Utils::ByteConverter::ToUInt16(data, packetStartIndex + 2));

    if (!ParentFrame()->QuickParse()) {
        if (headerFlags.Response()) addAttribute("Type", "Response");
        else addAttribute("Type", "Request");

        if (headerFlags.OperationCode() == static_cast<uint8_t>(HeaderFlags::OperationCodes::Query))
            addAttribute("Operation", "Standard Query");
        else if (headerFlags.OperationCode() == static_cast<uint8_t>(HeaderFlags::OperationCodes::InverseQuery))
            addAttribute("Operation", "Inverse Query");
    }

    if (headerFlags.OperationCode() < 5) {
        questionCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 4);
        answerCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 6);
        nameServerCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 8);
        additionalCount = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 10);

        if (questionCount > 0) {
            questionSectionByteCount = 0;
            std::vector<NameLabel> nameLabelList = GetNameLabelList(data, packetStartIndex, 12);
            for (const auto& label : nameLabelList) {
                questionSectionByteCount += label.LabelByteCount() + 1;
                questionNameDecoded << label.ToString();
            }
            questionSectionByteCount++; // 0x00 terminator

            if (packetStartIndex + 12 + questionSectionByteCount + 4 <= static_cast<int>(dataLen)) {
                questionType = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 12 + questionSectionByteCount);
                questionSectionByteCount += 2;
                questionClass = Utils::ByteConverter::ToUInt16(data, packetStartIndex + 12 + questionSectionByteCount);
                questionSectionByteCount += 2;
            }
        } else {
            questionSectionByteCount = 0;
        }

        int packetPositionIndex = packetStartIndex + 12 + questionSectionByteCount;
        for (int i = 0; i < answerCount; i++) {
            if (packetPositionIndex >= static_cast<int>(dataLen)) break;
            try {
                ResourceRecord* rr = new ResourceRecord(this, packetPositionIndex);
                answerRecords.push_back(rr);
                packetPositionIndex += rr->ByteCount();

                if (!ParentFrame()->QuickParse() && rr->Type() == static_cast<uint16_t>(RRTypes::HostAddress)) {
                    if (!rr->IP().isNull()) addAttribute("IP", rr->IP().toString());
                    if (!rr->DNS().isEmpty()) addAttribute("DNS", rr->DNS());
                }
            } catch (...) {
                break;
            }
        }
    }
}

DnsPacket::~DnsPacket() {
    for (auto rr : answerRecords) {
        delete rr;
    }
}

QString DnsPacket::QueriedDnsName() const {
    if (questionCount > 0 && !questionNameDecoded.empty()) {
        return questionNameDecoded.join(".");
    }
    return QString();
}

std::vector<AbstractPacket*> DnsPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
