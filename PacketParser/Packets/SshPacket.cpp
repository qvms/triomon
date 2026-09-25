#include "SshPacket.hpp"
#include <QRegularExpressionMatch>

namespace PacketParser {
namespace Packets {

SshPacket::SshPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "SSH")
{
    if (packetEndIndex - packetStartIndex > 100) {
        if (!ParentFrame()->QuickParse()) const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "Too long SSH banner"));
        return;
    } else if (packetEndIndex - packetStartIndex < 8) {
        if (!ParentFrame()->QuickParse()) const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "Too short SSH banner"));
        return;
    }
    
    int startIndex = packetStartIndex;
    QString prefix = Utils::ByteConverter::ReadString(parentFrame->Data(), parentFrame->DataLength(), startIndex, 4, false);
    if (prefix != "SSH-") {
        if (!ParentFrame()->QuickParse()) const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "Data does not start with SSH-"));
        return;
    }
    
    startIndex += 4;
    QString str = Utils::ByteConverter::ReadString(parentFrame->Data(), parentFrame->DataLength(), startIndex, packetEndIndex - startIndex + 1, false);
    while (str.endsWith('\r') || str.endsWith('\n')) {
        str.chop(1);
    }
    
    int hyphenIndex = str.indexOf('-');
    if (hyphenIndex != -1) {
        m_sshVersion = str.left(hyphenIndex);
        m_sshApplication = str.mid(hyphenIndex + 1);
    } else {
        m_sshVersion = str;
    }
}

bool SshPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;
    
    QRegularExpression regEx("^SSH-[12]\\.[0-9]");
    if (packetEndIndex - packetStartIndex > 100 || packetEndIndex - packetStartIndex < 8) {
        return false;
    }
    
    QString str = Utils::ByteConverter::ReadString(parentFrame->Data(), parentFrame->DataLength(), packetStartIndex, packetEndIndex - packetStartIndex + 1, false);
    if (!regEx.match(str).hasMatch()) {
        return false;
    }
    
    try {
        *result = new SshPacket(parentFrame, packetStartIndex, packetEndIndex);
    } catch (...) {
        *result = nullptr;
    }
    
    return *result != nullptr;
}

std::vector<AbstractPacket*> SshPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
