#include "AbstractTlsHandshakePacketHandler.hpp"
#include <QRegularExpression>
#include <QRegularExpressionMatch>

namespace PacketHandlerFramework {
namespace PacketHandlers {

AbstractTlsHandshakePacketHandler::AbstractTlsHandshakePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler, bool verifyX509Certificates)
    : AbstractPacketHandler(mainPacketHandler), verifyX509Certificates(verifyX509Certificates) {
    x509SubjectAndIssuerRegex = "(?<name>[a-zA-Z]+)=(?<value>[^,]+)"; // simplistic regex stub
}

void AbstractTlsHandshakePacketHandler::AddSubjectOrIssuerParameters(std::map<QString, QString>& parameters, const QString& x509Subject, const QString& parameterName) {
    QRegularExpression regex(x509SubjectAndIssuerRegex);
    QRegularExpressionMatchIterator i = regex.globalMatch(x509Subject);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString name = match.captured("name");
        QString val = match.captured("value");
        parameters[parameterName + " " + name] = val;
    }
}

void AbstractTlsHandshakePacketHandler::ParseCertificate(const std::vector<uint8_t>& certificate, void* transportPacket, int certChainIndex, bool transferIsClientToServer, void* fiveTuple) {
    // Stub: The full C# implementation uses .NET's X509Certificate2 which is complex to port 1:1 without OpenSSL/QtNetwork.
    // For now we will leave the structure ready.
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
