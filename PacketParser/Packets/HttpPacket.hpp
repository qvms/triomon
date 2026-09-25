#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <vector>
#include <QList>
#include <QPair>

namespace PacketParser {
namespace Packets {

class HttpPacket : public AbstractPacket {
public:
    enum class RequestMethods { GET, HEAD, POST, PUT, DELETE, TRACE, OPTIONS, CONNECT, none };
    enum class ContentEncodings { Gzip, Compress, Deflate, Identity };

private:
    bool messageTypeIsRequest;
    std::vector<QString> headerFields;
    std::vector<uint8_t> messageBody;

    RequestMethods requestMethod;
    QString requestedHost;
    QString requestedFileName;
    QString userAgentBanner;

    QString statusCode;
    QString serverBanner;
    QString contentType;
    int contentLength;
    QString contentEncoding;
    QString cookie;
    QString transferEncoding;
    QString wwwAuthenticateBasicRealm;
    QString authorizationCredentialsUsername;
    QString authorizationCredentialsPassword;

    bool packetHeaderIsComplete;

    HttpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);
    void ExtractHeaderField(const QString& headerField);
    QList<QPair<QString, QString>> GetUrlEncodedNameValueCollection(const QString& urlEncodedData) const;

public:
    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket*& result);

    bool MessageTypeIsRequest() const { return messageTypeIsRequest; }
    RequestMethods RequestMethod() const { return requestMethod; }
    QString RequestedHost() const { return requestedHost; }
    QString RequestedFileName() const { return requestedFileName; }
    QString UserAgentBanner() const { return userAgentBanner; }
    QString StatusCode() const { return statusCode; }
    QString ServerBanner() const { return serverBanner; }
    QString ContentType() const { return contentType; }
    int ContentLength() const { return contentLength; }
    QString ContentEncoding() const { return contentEncoding; }
    QString Cookie() const { return cookie; }
    QString TransferEncoding() const { return transferEncoding; }
    QString WwwAuthenticateBasicRealm() const { return wwwAuthenticateBasicRealm; }
    QString AuthorizationCredentialsUsername() const { return authorizationCredentialsUsername; }
    QString AuthorizationCredentialsPassword() const { return authorizationCredentialsPassword; }
    const std::vector<uint8_t>& MessageBody() const { return messageBody; }
    bool PacketHeaderIsComplete() const { return packetHeaderIsComplete; }

    QList<QPair<QString, QString>> GetQuerystringData() const;
    bool ContentIsComplete() const;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
