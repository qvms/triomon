#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <QStringList>
#include <QMap>
#include <QHostAddress>

namespace PacketParser {
namespace Packets {

class SipPacket : public AbstractPacket {
public:
    enum class RequestMethods {
        UNKNOWN,
        INVITE,
        ACK,
        OPTIONS,
        BYE,
        CANCEL,
        REGISTER,
        PRACK,
        SUBSCRIBE,
        NOTIFY,
        PUBLISH,
        INFO,
        REFER,
        MESSAGE,
        UPDATE
    };

    class SessionDescriptionProtocol {
    public:
        uint16_t Port;
        bool HasPort;
        QHostAddress IP;
        bool HasIP;
        QString Protocol;

        SessionDescriptionProtocol(const uint8_t* data, size_t dataLen, int index, const AbstractPacket* parentPacket);
    };

private:
    QString m_messageLine;
    RequestMethods m_requestMethod;
    QString m_to;
    QString m_from;
    QString m_callId;
    QString m_contact;
    QString m_contentType;
    int m_contentLength;
    QString m_userAgent;
    QMap<QString, QString> m_headerFields;
    int m_messageBodyStartIndex;
    SessionDescriptionProtocol* m_sdp;

    SipPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

public:
    ~SipPacket() { if (m_sdp) delete m_sdp; }

    QString MessageLine() const { return m_messageLine; }
    RequestMethods RequestMethod() const { return m_requestMethod; }
    QString To() const { return m_to; }
    QString From() const { return m_from; }
    QString CallId() const { return m_callId; }
    QString Contact() const { return m_contact; }
    QString ContentType() const { return m_contentType; }
    int ContentLength() const { return m_contentLength; }
    QString UserAgent() const { return m_userAgent; }
    const QMap<QString, QString>& HeaderFields() const { return m_headerFields; }
    int MessageBodyStartIndex() const { return m_messageBodyStartIndex; }
    const SessionDescriptionProtocol* SDP() const { return m_sdp; }

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
