#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include "../Utils.hpp"
#include <QString>
#include <QRegularExpression>

namespace PacketParser {
namespace Packets {

class SshPacket : public AbstractPacket, public virtual ISessionPacket {
private:
    QString m_sshVersion;
    QString m_sshApplication;

    SshPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

public:
    QString SshVersion() const { return m_sshVersion; }
    QString SshApplication() const { return m_sshApplication; }

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

    bool PacketHeaderIsComplete() const override { return true; }
    int ParsedBytesCount() const override { return PacketLength(); }
};

} // namespace Packets
} // namespace PacketParser
