#include "P0fOsFingerprintCollection.hpp"
#include "../../PacketParser/Utils/ByteConverter.hpp"

#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QStringList>

namespace PacketHandlerFramework {
namespace Fingerprints {

P0fOsFingerprintCollection::P0fFingerprint::P0fFingerprint(const QString& fingerprintString) {
    QStringList f = fingerprintString.split(':');
    if (f.size() >= 8) {
        this->windowSize = f[0];
        this->initialTtl = static_cast<uint8_t>(f[1].toUInt(nullptr, 10));
        this->dontFragment = (f[2] == "1");
        this->overallSynPacketSize = f[3];
        this->optionValue = f[4];
        this->quirksList = f[5];
        this->osGenre = f[6];
        this->osDetails = f[7];
    }
}

bool P0fOsFingerprintCollection::P0fFingerprint::Matches(PacketParser::Packets::IPv4Packet* ipPacket, PacketParser::Packets::TcpPacket* tcpPacket, uint8_t originalTimeToLive) const {
    if (this->windowSize.startsWith("S")) {
        int multipleMSS = this->windowSize.mid(1).toInt();
        int mss = 0;
        
        for (auto it = tcpPacket->OptionList().begin(); it != tcpPacket->OptionList().end(); ++it) {
            if (it.key() == PacketParser::Packets::TcpPacket::OptionKinds::MaximumSegmentSize) {
                mss = static_cast<int>(PacketParser::Utils::ByteConverter::ToUInt32(it.value().data(), 0));
            }
        }
        if (tcpPacket->WindowSize() != multipleMSS * mss)
            return false;
    } else if (this->windowSize.startsWith("T")) {
        int multipleMTU = this->windowSize.mid(1).toInt();
        int mtu = 1500;
        
        for (auto it = tcpPacket->OptionList().begin(); it != tcpPacket->OptionList().end(); ++it) {
            if (it.key() == PacketParser::Packets::TcpPacket::OptionKinds::MaximumSegmentSize) {
                mtu = static_cast<int>(PacketParser::Utils::ByteConverter::ToUInt32(it.value().data(), 0)) + 40;
            }
        }
        if (tcpPacket->WindowSize() != multipleMTU * mtu)
            return false;
    } else if (this->windowSize.startsWith("%")) {
        int modulo = this->windowSize.mid(1).toInt();
        if (modulo != 0 && tcpPacket->WindowSize() % modulo != 0)
            return false;
    } else if (this->windowSize.startsWith("*")) {
        // do nothing
    } else if (this->windowSize != QString::number(tcpPacket->WindowSize())) {
        return false;
    }
    
    if (originalTimeToLive != this->initialTtl)
        return false;
    if (ipPacket->DontFragmentFlag() != this->dontFragment)
        return false;
    if (this->overallSynPacketSize != "!" && this->overallSynPacketSize != QString::number(ipPacket->PacketByteCount()))
        return false;
        
    QStringList optionValues = this->optionValue.split(',');
    if (optionValues.size() != tcpPacket->OptionList().size() && !this->optionValue.isEmpty())
        return false;
        
    auto optionsKeys = tcpPacket->OptionList().keys();
    for (int i = 0; i < optionValues.size() && i < optionsKeys.size(); i++) {
        QString opt = optionValues[i];
        auto key = optionsKeys[i];
        auto value = tcpPacket->OptionList()[key];
        
        if (opt == "N") {
            if (key != PacketParser::Packets::TcpPacket::OptionKinds::NoOperation) return false;
        } else if (opt == "E") {
            if (key != PacketParser::Packets::TcpPacket::OptionKinds::EndOfOptionList) return false;
        } else if (opt.startsWith("W")) {
            if (key != PacketParser::Packets::TcpPacket::OptionKinds::WindowScaleFactor) return false;
            if (opt.length() > 1) {
                if (opt[1] == '%') {
                    int windowScaleFactor = static_cast<int>(PacketParser::Utils::ByteConverter::ToUInt32(value.data(), 0));
                    int modulo = opt.mid(2).toInt();
                    if (modulo != 0 && windowScaleFactor % modulo != 0) return false;
                } else if (opt[1] == '*') {
                    // do nothing
                } else {
                    int signatureScaleFactor = opt.mid(1).toInt();
                    int packetScaleFactor = static_cast<int>(PacketParser::Utils::ByteConverter::ToUInt32(value.data(), 0));
                    if (signatureScaleFactor != packetScaleFactor) return false;
                }
            }
        } else if (opt.startsWith("M")) {
            if (key != PacketParser::Packets::TcpPacket::OptionKinds::MaximumSegmentSize) return false;
            if (opt.length() > 1) {
                if (opt[1] == '%') {
                    int packetMSS = static_cast<int>(PacketParser::Utils::ByteConverter::ToUInt32(value.data(), 0));
                    int modulo = opt.mid(2).toInt();
                    if (modulo != 0 && packetMSS % modulo != 0) return false;
                } else if (opt[1] == '*') {
                    // do nothing
                } else {
                    int signatureMSS = opt.mid(1).toInt();
                    int packetMSS = static_cast<int>(PacketParser::Utils::ByteConverter::ToUInt32(value.data(), 0));
                    if (signatureMSS != packetMSS) return false;
                }
            }
        } else if (opt == "S") {
            if (key != PacketParser::Packets::TcpPacket::OptionKinds::SackPermitted) return false;
        } else if (opt == "K") {
            if (key != PacketParser::Packets::TcpPacket::OptionKinds::Sack) return false;
        } else if (opt == "T") {
            if (key != PacketParser::Packets::TcpPacket::OptionKinds::Timestamp) return false;
        } else if (opt == "T0") {
            if (key != PacketParser::Packets::TcpPacket::OptionKinds::Timestamp) return false;
            for (char b : value) {
                if (b != 0x00) return false;
            }
        } else if (opt.startsWith("?")) {
            // skip
        }
    }
    
    return true;
}

std::vector<std::shared_ptr<P0fOsFingerprintCollection::P0fFingerprint>> P0fOsFingerprintCollection::GetFingerprintList(const QString& fingerprintFile) {
    std::vector<std::shared_ptr<P0fFingerprint>> fingerprintList;
    
    QFile file(fingerprintFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.length() > 10 && line[0] != '#') {
                auto fingerprint = std::make_shared<P0fFingerprint>(line);
                fingerprintList.push_back(fingerprint);
                timeToLiveExists[fingerprint->InitialTTL()] = true;
            }
        }
    }
    
    return fingerprintList;
}

P0fOsFingerprintCollection::P0fOsFingerprintCollection(const QString& synFingerprintFile, const QString& synAckFingerprintFile)
    : maxTtlDistance(31) {
    synOsFingerprints = GetFingerprintList(synFingerprintFile);
    synAckOsFingerprints = GetFingerprintList(synAckFingerprintFile);
}

bool P0fOsFingerprintCollection::TryGetOperatingSystems(QList<DeviceFingerprint>& osList, const QList<PacketParser::Packets::AbstractPacket*>& packetList) {
    PacketParser::Packets::IPv4Packet* ipv4Packet = nullptr;
    PacketParser::Packets::TcpPacket* tcpPacket = nullptr;

    for (auto* p : packetList) {
        if (auto ipv4 = dynamic_cast<PacketParser::Packets::IPv4Packet*>(p)) {
            ipv4Packet = ipv4;
        } else if (auto tcp = dynamic_cast<PacketParser::Packets::TcpPacket*>(p)) {
            tcpPacket = tcp;
        }
    }

    if (ipv4Packet != nullptr && tcpPacket != nullptr && tcpPacket->FlagBits().Synchronize()) {
        const std::vector<std::shared_ptr<P0fFingerprint>>& osFingerprintList = 
            tcpPacket->FlagBits().Acknowledgement() ? synAckOsFingerprints : synOsFingerprints;

        uint8_t originalTTL = GetOriginalTimeToLive(ipv4Packet, tcpPacket);
        
        for (const auto& fingerprint : osFingerprintList) {
            if (fingerprint->Matches(ipv4Packet, tcpPacket, originalTTL)) {
                osList.clear();
                osList.append(DeviceFingerprint(fingerprint->OS().toStdString()));
                return true;
            }
        }
    }

    return false;
}

uint8_t P0fOsFingerprintCollection::GetOriginalTimeToLive(PacketParser::Packets::IPv4Packet* ipv4Packet, PacketParser::Packets::TcpPacket* tcpPacket) {
    for (int ttlOffset = 0; ttlOffset < maxTtlDistance && ipv4Packet->TimeToLive() + ttlOffset <= 255; ttlOffset++) {
        if (timeToLiveExists[ipv4Packet->TimeToLive() + ttlOffset]) {
            if (ipv4Packet->TimeToLive() + ttlOffset == AbstractTtlDistanceCalculator::GetOriginalTimeToLive(ipv4Packet->TimeToLive()))
                return static_cast<uint8_t>(ipv4Packet->TimeToLive() + ttlOffset);
        }
    }
    
    for (int ttlOffset = 0; ttlOffset < maxTtlDistance && ipv4Packet->TimeToLive() + ttlOffset <= 255; ttlOffset++) {
        if (timeToLiveExists[ipv4Packet->TimeToLive() + ttlOffset]) {
            return static_cast<uint8_t>(ipv4Packet->TimeToLive() + ttlOffset);
        }
    }
    
    return AbstractTtlDistanceCalculator::GetOriginalTimeToLive(ipv4Packet->TimeToLive());
}

uint8_t P0fOsFingerprintCollection::GetTtlDistance(PacketParser::Packets::IPv4Packet* ipv4Packet, PacketParser::Packets::TcpPacket* tcpPacket) {
    uint8_t originalTimeToLive = GetOriginalTimeToLive(ipv4Packet, tcpPacket);
    return originalTimeToLive - ipv4Packet->TimeToLive();
}

bool P0fOsFingerprintCollection::TryGetTtlDistance(uint8_t& ttlDistance, const std::vector<void*>& packetList) {
    PacketParser::Packets::IPv4Packet* ipv4Packet = nullptr;
    PacketParser::Packets::TcpPacket* tcpPacket = nullptr;

    for (void* p : packetList) {
        auto* abstractPacket = static_cast<PacketParser::Packets::AbstractPacket*>(p);
        if (auto ipv4 = dynamic_cast<PacketParser::Packets::IPv4Packet*>(abstractPacket)) {
            ipv4Packet = ipv4;
        } else if (auto tcp = dynamic_cast<PacketParser::Packets::TcpPacket*>(abstractPacket)) {
            tcpPacket = tcp;
        }
    }

    if (ipv4Packet != nullptr && tcpPacket != nullptr) {
        ttlDistance = GetTtlDistance(ipv4Packet, tcpPacket);
        return true;
    } else {
        ttlDistance = 0;
        return false;
    }
}

uint8_t P0fOsFingerprintCollection::GetTtlDistance(uint8_t ipTimeToLive) {
    uint8_t originalTimeToLive = AbstractTtlDistanceCalculator::GetOriginalTimeToLive(ipTimeToLive);
    return originalTimeToLive - ipTimeToLive;
}

} // namespace Fingerprints
} // namespace PacketHandlerFramework
