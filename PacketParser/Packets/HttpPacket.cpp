#include "HttpPacket.hpp"
#include "../Utils.hpp"
#include <QUrl>
#include <QUrlQuery>
#include <QByteArray>

namespace PacketParser {
namespace Packets {

bool HttpPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result) {
    result = nullptr;

    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex >= static_cast<int>(dataLen)) return false;

    int dataIndex = packetStartIndex;
    QString startLine = Utils::ByteConverter::ReadLine(data, dataIndex);
    
    if (startLine.isNull()) return false;
    if (startLine.length() > 2048) return false;
    if (!(startLine.startsWith("GET") || startLine.startsWith("HEAD") || startLine.startsWith("POST") || 
          startLine.startsWith("PUT") || startLine.startsWith("DELETE") || startLine.startsWith("TRACE") || 
          startLine.startsWith("OPTIONS") || startLine.startsWith("CONNECT") || startLine.startsWith("HTTP"))) {
        return false;
    }

    try {
        result = new HttpPacket(parentFrame, packetStartIndex, packetEndIndex);
        return true;
    } catch (...) {
        result = nullptr;
        return false;
    }
}

HttpPacket::HttpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "HTTP")
    , contentLength(-1)
    , packetHeaderIsComplete(false)
{
    const uint8_t* data = parentFrame->Data();
    int dataIndex = packetStartIndex;

    QString startLine = Utils::ByteConverter::ReadLine(data, dataIndex);
    if (startLine.isNull()) throw std::runtime_error("HTTP packet does not contain a valid start line. Probably a false HTTP positive");
    if (startLine.length() > 2048) throw std::runtime_error("HTTP start line is longer than 255 bytes. Probably a false HTTP positive");

    if (startLine.startsWith("GET")) {
        messageTypeIsRequest = true;
        requestMethod = RequestMethods::GET;
    } else if (startLine.startsWith("HEAD")) {
        messageTypeIsRequest = true;
        requestMethod = RequestMethods::HEAD;
    } else if (startLine.startsWith("POST")) {
        messageTypeIsRequest = true;
        requestMethod = RequestMethods::POST;
    } else if (startLine.startsWith("PUT")) {
        messageTypeIsRequest = true;
        requestMethod = RequestMethods::PUT;
    } else if (startLine.startsWith("DELETE")) {
        messageTypeIsRequest = true;
        requestMethod = RequestMethods::DELETE;
    } else if (startLine.startsWith("TRACE")) {
        messageTypeIsRequest = true;
        requestMethod = RequestMethods::TRACE;
    } else if (startLine.startsWith("OPTIONS")) {
        messageTypeIsRequest = true;
        requestMethod = RequestMethods::OPTIONS;
    } else if (startLine.startsWith("CONNECT")) {
        messageTypeIsRequest = true;
        requestMethod = RequestMethods::CONNECT;
    } else if (startLine.startsWith("HTTP")) {
        messageTypeIsRequest = false;
        requestMethod = RequestMethods::none;
    } else {
        throw std::runtime_error("Incorrect HTTP Message Type or Request Method");
    }

    if (messageTypeIsRequest) {
        int space1 = startLine.indexOf(' ');
        if (space1 != -1) {
            int space2 = startLine.indexOf(' ', space1 + 1);
            if (space2 != -1) {
                requestedFileName = startLine.mid(space1 + 1, space2 - space1 - 1);
            }
        }
    } else {
        if (startLine.startsWith("HTTP/1.")) {
            statusCode = startLine.mid(9, 3);
        }
    }

    while (true) {
        QString headerLine = Utils::ByteConverter::ReadLine(data, dataIndex);
        if (headerLine.isNull()) {
            break;
        } else if (headerLine.length() > 0) {
            headerFields.push_back(headerLine);
            ExtractHeaderField(headerLine);
        } else {
            packetHeaderIsComplete = true;
            break;
        }
    }

    if (packetHeaderIsComplete && dataIndex <= packetEndIndex) {
        int bodyLength = packetEndIndex - dataIndex + 1;
        if (bodyLength > 0 && dataIndex + bodyLength <= static_cast<int>(parentFrame->DataLength())) {
            messageBody.resize(bodyLength);
            memcpy(messageBody.data(), data + dataIndex, bodyLength);
        }
    }
}

void HttpPacket::ExtractHeaderField(const QString& headerField) {
    if (headerField.startsWith("Host: ", Qt::CaseInsensitive)) {
        requestedHost = headerField.mid(6);
        addAttribute("Requested Host", requestedHost);
    } else if (headerField.startsWith("User-Agent: ", Qt::CaseInsensitive)) {
        userAgentBanner = headerField.mid(12);
        addAttribute("User-Agent", userAgentBanner);
    } else if (headerField.startsWith("Server: ", Qt::CaseInsensitive)) {
        serverBanner = headerField.mid(8);
        addAttribute("Server banner", serverBanner);
    } else if (headerField.startsWith("Cookie: ", Qt::CaseInsensitive)) {
        cookie = headerField.mid(8);
        addAttribute("Cookie", cookie);
    } else if (headerField.startsWith("Content-Type: ", Qt::CaseInsensitive)) {
        contentType = headerField.mid(14);
    } else if (headerField.startsWith("Content-Length: ", Qt::CaseInsensitive)) {
        contentLength = headerField.mid(16).toInt();
    } else if (headerField.startsWith("Content-Encoding: ", Qt::CaseInsensitive)) {
        contentEncoding = headerField.mid(18);
    } else if (headerField.startsWith("Transfer-Encoding: ", Qt::CaseInsensitive)) {
        transferEncoding = headerField.mid(19);
    } else if (headerField.startsWith("WWW-Authenticate: Basic realm=", Qt::CaseInsensitive)) {
        wwwAuthenticateBasicRealm = headerField.mid(31, headerField.length() - 32);
    } else if (headerField.startsWith("Proxy-Authenticate: Basic realm=", Qt::CaseInsensitive)) {
        wwwAuthenticateBasicRealm = headerField.mid(33, headerField.length() - 34);
    } else if (headerField.startsWith("Authorization: Basic ", Qt::CaseInsensitive)) {
        try {
            QString base64string = headerField.mid(21);
            QByteArray decoded = QByteArray::fromBase64(base64string.toUtf8());
            QString s = QString::fromUtf8(decoded);
            int colonIndex = s.indexOf(':');
            if (colonIndex != -1) {
                authorizationCredentialsUsername = s.left(colonIndex);
                if (colonIndex + 1 < s.length()) {
                    authorizationCredentialsPassword = s.mid(colonIndex + 1);
                } else {
                    authorizationCredentialsPassword = "";
                }
            }
        } catch (...) {
            // Log error internally in C# it adds to Frame.Errors
        }
    }
}

QList<QPair<QString, QString>> HttpPacket::GetUrlEncodedNameValueCollection(const QString& urlEncodedData) const {
    QList<QPair<QString, QString>> result;
    QUrlQuery query(urlEncodedData);
    for (const auto& pair : query.queryItems()) {
        result.push_back({QUrl::fromPercentEncoding(pair.first.toUtf8()), QUrl::fromPercentEncoding(pair.second.toUtf8())});
    }
    return result;
}

QList<QPair<QString, QString>> HttpPacket::GetQuerystringData() const {
    if (!requestedFileName.isNull() && requestedFileName.contains('?')) {
        int qMark = requestedFileName.indexOf('?');
        return GetUrlEncodedNameValueCollection(requestedFileName.mid(qMark + 1));
    }
    return QList<QPair<QString, QString>>();
}

bool HttpPacket::ContentIsComplete() const {
    if (contentLength == 0) return true;
    if (messageBody.empty()) return false;
    return static_cast<int>(messageBody.size()) >= contentLength;
}

std::vector<AbstractPacket*> HttpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
