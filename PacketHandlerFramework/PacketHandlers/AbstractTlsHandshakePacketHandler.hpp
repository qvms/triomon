#pragma once

#include "AbstractPacketHandler.hpp"
#include <QRegularArea>
#include <QString>
#include <vector>
#include <map>

namespace PacketHandlerFramework {
namespace PacketHandlers {

class AbstractTlsHandshakePacketHandler : public AbstractPacketHandler {
private:
    // Regex or parsing pattern for Subject/Issuer, stubbed as string for now
    QString x509SubjectAndIssuerRegex;
    bool verifyX509Certificates;
    std::map<QString, QString> abuseChX509CertificateFingerprints;

protected:
    void AddSubjectOrIssuerParameters(std::map<QString, QString>& parameters, const QString& x509Subject, const QString& parameterName);

public:
    AbstractTlsHandshakePacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler, bool verifyX509Certificates = false);
    virtual ~AbstractTlsHandshakePacketHandler() = default;

    // A method likely meant to parse the certificate 
    void ParseCertificate(const std::vector<uint8_t>& certificate, void* transportPacket, int certChainIndex, bool transferIsClientToServer, void* fiveTuple);
};

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
