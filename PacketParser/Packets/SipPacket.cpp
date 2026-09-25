#include "SipPacket.hpp"
#include "../Utils.hpp"
#include <QStringList>

namespace PacketParser {
namespace Packets {

SipPacket::SessionDescriptionProtocol::SessionDescriptionProtocol(const uint8_t* data, size_t dataLen, int index, const AbstractPacket* parentPacket)
    : Port(0), HasPort(false), HasIP(false)
{
    QString headerLine = "dummy value";
    
    while (index < parentPacket->PacketEndIndex()) {
        headerLine = Utils::ByteConverter::ReadLine(data, dataLen, index, true);
        if (headerLine.contains("=")) {
            int eqIndex = headerLine.indexOf('=');
            QString headerName = headerLine.left(eqIndex).trimmed();
            QString headerValue = headerLine.mid(eqIndex + 1).trimmed();
            
            if (headerName.length() == 1 && headerValue.length() > 0) {
                if (headerName == "c" && headerValue.startsWith("IN", Qt::CaseInsensitive)) {
                    if (headerValue.contains(" ")) {
                        QStringList parts = headerValue.split(' ', Qt::SkipEmptyParts);
                        if (parts.size() > 2) {
                            QHostAddress ip(parts[2].trimmed());
                            if (!ip.isNull()) {
                                IP = ip;
                                HasIP = true;
                            }
                        }
                    }
                } else if (headerName == "m" && headerValue.startsWith("audio", Qt::CaseInsensitive)) {
                    if (headerValue.contains(" ")) {
                        QStringList parts = headerValue.split(' ', Qt::SkipEmptyParts);
                        if (parts.size() > 1) {
                            bool ok;
                            uint16_t port = parts[1].trimmed().toUShort(&ok);
                            if (ok) {
                                Port = port;
                                HasPort = true;
                            }
                        }
                        if (parts.size() > 2) {
                            Protocol = parts[2].trimmed();
                        }
                    }
                }
            }
        }
    }
}

SipPacket::SipPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "SIP")
    , m_requestMethod(RequestMethods::UNKNOWN)
    , m_contentLength(0)
    , m_messageBodyStartIndex(0)
    , m_sdp(nullptr)
{
    int index = packetStartIndex;
    m_messageLine = Utils::ByteConverter::ReadLine(parentFrame->Data(), parentFrame->DataLength(), index, true);
    
    QString requestMethodString = m_messageLine.split(' ').first();
    if (requestMethodString == "INVITE") m_requestMethod = RequestMethods::INVITE;
    else if (requestMethodString == "ACK") m_requestMethod = RequestMethods::ACK;
    else if (requestMethodString == "OPTIONS") m_requestMethod = RequestMethods::OPTIONS;
    else if (requestMethodString == "BYE") m_requestMethod = RequestMethods::BYE;
    else if (requestMethodString == "CANCEL") m_requestMethod = RequestMethods::CANCEL;
    else if (requestMethodString == "REGISTER") m_requestMethod = RequestMethods::REGISTER;
    else if (requestMethodString == "PRACK") m_requestMethod = RequestMethods::PRACK;
    else if (requestMethodString == "SUBSCRIBE") m_requestMethod = RequestMethods::SUBSCRIBE;
    else if (requestMethodString == "NOTIFY") m_requestMethod = RequestMethods::NOTIFY;
    else if (requestMethodString == "PUBLISH") m_requestMethod = RequestMethods::PUBLISH;
    else if (requestMethodString == "INFO") m_requestMethod = RequestMethods::INFO;
    else if (requestMethodString == "REFER") m_requestMethod = RequestMethods::REFER;
    else if (requestMethodString == "MESSAGE") m_requestMethod = RequestMethods::MESSAGE;
    else if (requestMethodString == "UPDATE") m_requestMethod = RequestMethods::UPDATE;
    
    if (!ParentFrame()->QuickParse()) {
        addAttribute("Message Line", m_messageLine);
    }
    
    QString headerLine = "dummy value";
    while (index < PacketEndIndex() && headerLine.length() > 0) {
        headerLine = Utils::ByteConverter::ReadLine(parentFrame->Data(), parentFrame->DataLength(), index, true);
        if (headerLine.contains(":")) {
            int colonIndex = headerLine.indexOf(':');
            QString headerName = headerLine.left(colonIndex);
            QString headerValue = headerLine.mid(colonIndex + 1).trimmed();
            
            if (headerName.length() > 0 && headerValue.length() > 0) {
                m_headerFields[headerName] = headerValue;
                
                if (headerName.compare("To", Qt::CaseInsensitive) == 0 || headerName == "t") m_to = headerValue;
                else if (headerName.compare("From", Qt::CaseInsensitive) == 0 || headerName == "f") m_from = headerValue;
                else if (headerName.compare("Contact", Qt::CaseInsensitive) == 0 || headerName == "m") m_contact = headerValue;
                else if (headerName.compare("Call-ID", Qt::CaseInsensitive) == 0 || headerName == "i") m_callId = headerValue;
                else if (headerName.compare("Content-Type", Qt::CaseInsensitive) == 0 || headerName == "c") m_contentType = headerValue;
                else if (headerName.compare("Content-Length", Qt::CaseInsensitive) == 0 || headerName == "l") {
                    bool ok;
                    int cl = headerValue.toInt(&ok);
                    if (ok) m_contentLength = cl;
                }
                else if (headerName.compare("User-Agent", Qt::CaseInsensitive) == 0) m_userAgent = headerValue;
            }
        }
    }
    
    m_messageBodyStartIndex = index;
    if (m_contentLength > 0) {
        if (index + m_contentLength < packetEndIndex + 1) {
            if (!ParentFrame()->QuickParse()) {
                const_cast<Frame*>(parentFrame)->addError(Frame::Error(parentFrame, packetStartIndex, packetEndIndex, "Incomplete SIP packet"));
            }
            return;
        } else if (index + m_contentLength > packetEndIndex + 1) {
            setPacketEndIndex(index + m_contentLength - 1);
        }
        
        if (m_contentType.compare("application/sdp", Qt::CaseInsensitive) == 0) {
            m_sdp = new SessionDescriptionProtocol(parentFrame->Data(), parentFrame->DataLength(), index, this);
        }
    }
}

bool SipPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result) {
    if (!result) return false;
    *result = nullptr;
    
    if (packetEndIndex - packetStartIndex < 10) return false;
    
    int index = packetStartIndex;
    QString messageLine = Utils::ByteConverter::ReadLine(parentFrame->Data(), parentFrame->DataLength(), index, true);
    
    if (messageLine.isEmpty()) return false;
    
    if (messageLine.startsWith("SIP/2.0") ||
        messageLine.startsWith("INVITE") ||
        messageLine.startsWith("ACK") ||
        messageLine.startsWith("OPTIONS") ||
        messageLine.startsWith("BYE") ||
        messageLine.startsWith("CANCEL") ||
        messageLine.startsWith("REGISTER") ||
        messageLine.startsWith("PRACK") ||
        messageLine.startsWith("SUBSCRIBE") ||
        messageLine.startsWith("NOTIFY") ||
        messageLine.startsWith("PUBLISH") ||
        messageLine.startsWith("INFO") ||
        messageLine.startsWith("REFER") ||
        messageLine.startsWith("MESSAGE") ||
        messageLine.startsWith("UPDATE")) {
        
        try {
            *result = new SipPacket(parentFrame, packetStartIndex, packetEndIndex);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    return false;
}

std::vector<AbstractPacket*> SipPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
