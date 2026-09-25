#include "DnsPacketHandler.hpp"
#include "../../PacketHandlerFramework/Events/ParametersEventArgs.hpp"
#include "../../PacketHandlerFramework/Events/NetworkHostEventArgs.hpp"
#include "../../PacketHandlerFramework/Events/DnsRecordEventArgs.hpp"
#include "../../PacketHandlerFramework/MainPacketHandler.hpp"
#include <QVariantMap>
#include <QRegularExpression>

namespace PacketHandlerFramework {
namespace PacketHandlers {

const QSet<QString> DnsPacketHandler::DNSBL::DNSBL_DOMAINS = {
    "access.redhawk.org",
    "all.s5h.net",
    "b.barracudacentral.org",
    "blackholes.mail-abuse.org",
    "blacklist.woody.ch",
    "bl.mailspike.net",
    "bl.spamcop.net",
    "bogons.cymru.com",
    "cbl.abuseat.org",
    "cbl.anti-spam.org.cn",
    "cdl.anti-spam.org.cn",
    "combined.abuse.ch",
    "csi.cloudmark.com",
    "db.wpbl.info",
    "dnsbl-1.uceprotect.net",
    "dnsbl-2.uceprotect.net",
    "dnsbl-3.uceprotect.net",
    "dnsbl.dronebl.org",
    "dnsbl.inps.de",
    "dnsbl.sorbs.net",
    "drone.abuse.ch",
    "dsn.rfc-ignorant.org",
    "duinv.aupads.org",
    "dul.dnsbl.sorbs.net",
    "dyna.spamrats.com",
    "httpbl.abuse.ch",
    "http.dnsbl.sorbs.net",
    "ips.backscatterer.org",
    "ix.dnsbl.manitu.net",
    "korea.services.net",
    "misc.dnsbl.sorbs.net",
    "multi.surbl.org",
    "netblock.pedantic.org",
    "noptr.spamrats.com",
    "opm.tornevall.org",
    "orvedb.aupads.org",
    "pbl.spamhaus.org",
    "proxy.bl.gweep.ca",
    "psbl.surriel.com",
    "query.senderbase.org",
    "rbl.efnetrbl.org",
    "rbl.interserver.net",
    "rbl-plus.mail-abuse.org",
    "rbl.spamlab.com",
    "rbl.suresupport.com",
    "relays.bl.gweep.ca",
    "relays.mail-abuse.org",
    "relays.nether.net",
    "sbl.spamhaus.org",
    "short.rbl.jp",
    "singular.ttk.pte.hu",
    "smtp.dnsbl.sorbs.net",
    "socks.dnsbl.sorbs.net",
    "spam.abuse.ch",
    "spambot.bls.digibase.ca",
    "spam.dnsbl.anonmails.de",
    "spam.dnsbl.sorbs.net",
    "spamguard.leadmon.net",
    "spamrbl.imp.ch",
    "spamsources.fabel.dk",
    "spam.spamrats.com",
    "tor.dan.me.uk",
    "truncate.gbudb.net",
    "ubl.lashback.com",
    "ubl.unsubscore.com",
    "virbl.bit.nl",
    "virus.rbl.jp",
    "web.dnsbl.sorbs.net",
    "wormrbl.imp.ch",
    "xbl.spamhaus.org",
    "zen.spamhaus.org",
    "z.mailspike.net",
    "zombie.dnsbl.sorbs.net"
};

bool DnsPacketHandler::DNSBL::TryParse(const QString& domain, QHostAddress& ip, QString& service) {
    QRegularExpression re("^(?<revip>[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)\\.(?<service>[a-zA-Z\\-\\.]*)");
    QRegularExpressionMatch match = re.match(domain);
    if(match.hasMatch()) {
        QString revip = match.captured("revip");
        service = match.captured("service");
        if(!revip.isEmpty() && DNSBL_DOMAINS.contains(service)) {
            QStringList parts = revip.split('.');
            std::reverse(parts.begin(), parts.end());
            QString ipString = parts.join('.');
            return ip.setAddress(ipString);
        }
    }
    return false;
}

DnsPacketHandler::DnsPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

void DnsPacketHandler::ExtractDataFromPacket(void* transportPacket) {
    // Handled by IPacketHandler & ITcpSessionPacketHandler interface simulation
}

void DnsPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkHost>& sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::DnsPacket> dnsPacket;
    std::shared_ptr<PacketParser::Packets::ITransportLayerPacket> transportLayerPacket;
    std::shared_ptr<PacketParser::Packets::IIPPacket> ipPacket;

    for(auto& p : packetList) {
        if(!dnsPacket) dnsPacket = std::dynamic_pointer_cast<PacketParser::Packets::DnsPacket>(p);
        if(!transportLayerPacket) transportLayerPacket = std::dynamic_pointer_cast<PacketParser::Packets::ITransportLayerPacket>(p);
        if(!ipPacket) ipPacket = std::dynamic_pointer_cast<PacketParser::Packets::IIPPacket>(p);
    }

    if (dnsPacket) {
        if (dnsPacket->Flags().Response()) {
            QList<std::shared_ptr<PacketParser::Packets::DnsPacket::ResourceRecord>> rrList;
            for(auto rr : dnsPacket->AnswerRecords()) {
                rrList.append(std::shared_ptr<PacketParser::Packets::DnsPacket::ResourceRecord>(rr, [](PacketParser::Packets::DnsPacket::ResourceRecord*){})); // Do not delete, owned by dnsPacket
            }

            if (!rrList.isEmpty()) {
                ExtractDomainNameRecords(dnsPacket, rrList, sourceHost, destinationHost, ipPacket, transportLayerPacket);
            }
            else {
                if (!dnsPacket->QueriedDnsName().isEmpty()) {
                    // TODO: Need ResponseWithErrorCode implementation in C++ DnsPacket 
                    auto dre = std::make_shared<PacketHandlerFramework::Events::DnsRecordEventArgs>(nullptr, sourceHost, destinationHost, ipPacket, transportLayerPacket);
                    GetMainPacketHandler()->OnDnsRecordDetected(dre);
                }
            }
            
            // Note: additionalRecords in DnsPacket are not exposed in C++ DnsPacket currently
        } else { // DNS request
            if (!dnsPacket->QueriedDnsName().isEmpty()) {
                sourceHost->AddQueriedDnsName(dnsPacket->QueriedDnsName());
                QHostAddress ipServiceIp;
                QString ipServiceService;
                if (DNSBL::TryParse(dnsPacket->QueriedDnsName(), ipServiceIp, ipServiceService)) {
                    sourceHost->AddNumberedExtraDetail("DNSBL lookup", ipServiceIp.toString() + " through " + dnsPacket->QueriedDnsName());
                    if (transportLayerPacket) {
                        QVariantMap parms;
                        parms.insert("DNSBL lookup at " + ipServiceService, ipServiceIp.toString());
                        auto pe = std::make_shared<PacketHandlerFramework::Events::ParametersEventArgs>(
                            dnsPacket->GetParentFrame()->GetFrameNumber(), sourceHost, destinationHost, 
                            transportLayerPacket->TransportProtocol(), transportLayerPacket->SourcePort(), transportLayerPacket->DestinationPort(), 
                            parms, dnsPacket->GetParentFrame()->GetTimestamp(), "DNS query");
                        GetMainPacketHandler()->OnParametersDetected(pe);
                    }
                }
            }
        }
    }
}

int DnsPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    int parsedBytes = 0;
    for(auto& p : packetList) {
        auto dp = std::dynamic_pointer_cast<PacketParser::Packets::DnsPacket>(p);
        if(dp) {
            parsedBytes += dp->GetPacketLength();
            // parsedBytes += dp->SkippedBytes(); // SkippedBytes not available in C++ DnsPacket yet
        }
    }

    std::shared_ptr<PacketParser::NetworkHost> sourceHost;
    std::shared_ptr<PacketParser::NetworkHost> destinationHost;
    if (transferIsClientToServer) {
        sourceHost = tcpSession->GetClientHost();
        destinationHost = tcpSession->GetServerHost();
    }
    else {
        sourceHost = tcpSession->GetServerHost();
        destinationHost = tcpSession->GetClientHost();
    }
    ExtractData(sourceHost, destinationHost, packetList);
    return parsedBytes;
}

void DnsPacketHandler::ExtractDomainNameRecords(std::shared_ptr<PacketParser::Packets::DnsPacket> dnsPacket, const QList<std::shared_ptr<PacketParser::Packets::DnsPacket::ResourceRecord>>& rr, std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, std::shared_ptr<PacketParser::Packets::IIPPacket> ipPacket, std::shared_ptr<PacketParser::Packets::ITransportLayerPacket> transportLayerPacket) {
    QMap<QString, QString> cNamePointers;
    for (auto r : rr) {
        QHostAddress ip = r->IP();
        if (!ip.isNull() && ip != QHostAddress::Any) {
            if (!GetMainPacketHandler()->GetNetworkHostList()->ContainsIP(ip)) {
                auto host = std::make_shared<PacketParser::NetworkHost>(ip);
                host->AddHostName(r->DNS(), dnsPacket->GetPacketTypeDescription());
                GetMainPacketHandler()->GetNetworkHostList()->Add(host);
                
                auto nhe = std::make_shared<PacketHandlerFramework::Events::NetworkHostEventArgs>(host);
                GetMainPacketHandler()->OnNetworkHostDetected(nhe);
            }
            else {
                GetMainPacketHandler()->GetNetworkHostList()->GetNetworkHost(ip)->AddHostName(r->DNS(), dnsPacket->GetPacketTypeDescription());
            }
            if (cNamePointers.contains(r->DNS())) {
                GetMainPacketHandler()->GetNetworkHostList()->GetNetworkHost(ip)->AddHostName(cNamePointers[r->DNS()], dnsPacket->GetPacketTypeDescription());
            }
        }
        else if (r->Type() == static_cast<uint16_t>(PacketParser::Packets::DnsPacket::RRTypes::CNAME)) {
            cNamePointers.insert(r->PrimaryName(), r->DNS());
        }

        auto dre = std::make_shared<PacketHandlerFramework::Events::DnsRecordEventArgs>(r, sourceHost, destinationHost, ipPacket, transportLayerPacket);
        GetMainPacketHandler()->OnDnsRecordDetected(dre);
    }
}

void DnsPacketHandler::Reset() {
    // do nothing since this class holds no state
}

}
}
