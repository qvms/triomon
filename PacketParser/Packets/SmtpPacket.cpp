#include "SmtpPacket.hpp"
#include "FtpPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

bool SmtpPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, uint16_t sourcePort, AbstractPacket*& smtpPacket) {
    smtpPacket = nullptr;
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (clientToServer) {
        if (packetEndIndex - packetStartIndex > 80) {
            int index = packetStartIndex;
            QString line = Utils::ByteConverter::ReadLine(data, index);
            if (line.isNull()) return false;
        }
    } else {
        int index = packetStartIndex;
        QString firstLine = Utils::ByteConverter::ReadLine(data, index);
        QString line = firstLine;
        
        while (!line.isEmpty()) {
            QString lowerLine = line.toLower();
            if (lowerLine.contains("smtp") || lowerLine.contains("mail")) {
                smtpPacket = new SmtpPacket(parentFrame, packetStartIndex, packetEndIndex, clientToServer);
                return true;
            } else if (lowerLine.startsWith("220") && FtpPacket::ContainsLikelyFtpBanner(lowerLine)) {
                return false;
            }
            line = Utils::ByteConverter::ReadLine(data, index);
        }

        if (!firstLine.isNull() && firstLine.startsWith("220") && sourcePort == 21) {
            return false;
        }
    }

    if (packetEndIndex < static_cast<int>(dataLen) && data[packetEndIndex] == 0 && packetEndIndex - packetStartIndex >= 3) {
        for (int i = packetStartIndex; i <= packetEndIndex; i++) {
            if (data[i] == 0) {
                if (i - packetStartIndex >= 3) {
                    return false;
                } else break;
            } else if (data[i] < 0x20 || data[i] > 0x7e) {
                break;
            }
        }
    }
    
    smtpPacket = new SmtpPacket(parentFrame, packetStartIndex, packetEndIndex, clientToServer);
    return true;
}

SmtpPacket::SmtpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "SMTP")
    , clientToServer(clientToServer)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();
    int index = PacketStartIndex();

    if (clientToServer) {
        while (index < packetEndIndex && requestCommandAndArgumentList.size() < 1000) {
            QString line = Utils::ByteConverter::ReadLine(data, index);
            if (line.isEmpty()) break;

            QString requestCommand;
            QString requestArgument;

            int spaceIdx = line.indexOf(' ');
            if (spaceIdx != -1) {
                requestCommand = line.left(spaceIdx);
                if (line.length() > spaceIdx + 1) {
                    requestArgument = line.mid(spaceIdx + 1);
                }
            } else if (line.length() == 4) {
                requestCommand = line;
            } else {
                // Simplified enum check
                requestCommand = line;
            }

            if (!requestCommand.isNull()) {
                requestCommandAndArgumentList.push_back({requestCommand, requestArgument});
            } else {
                break;
            }
        }
    } else {
        while (index < packetEndIndex && replyList.size() < 1000) {
            QString replyArgument;
            QString line = Utils::ByteConverter::ReadLine(data, index);
            if (line.isEmpty()) break;
            else if (replyList.empty() && line.startsWith("220") && line.toLower().contains("ftp") && !line.toLower().contains("smtp") && !line.toLower().contains("mail")) {
                // Abort
                break;
            }

            bool ok;
            int replyCode = line.left(3).toInt(&ok);
            if (!ok) break;
            else if (line.length() > 4) replyArgument = line.mid(4);
            
            replyList.push_back({replyCode, replyArgument});
        }
    }
}

std::vector<int> SmtpPacket::ReplyCodes() const {
    std::vector<int> codes;
    for (const auto& pair : replyList) {
        codes.push_back(pair.first);
    }
    return codes;
}

QString SmtpPacket::ReadLine() const {
    int index = PacketStartIndex();
    return Utils::ByteConverter::ReadLine(ParentFrame()->Data(), index);
}

std::vector<AbstractPacket*> SmtpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
