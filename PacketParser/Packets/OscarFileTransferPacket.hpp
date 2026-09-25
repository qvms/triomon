#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include "../Utils.hpp"
#include <QString>

namespace PacketParser {
namespace Packets {

class OscarFileTransferPacket : public AbstractPacket, public virtual ISessionPacket {
public:
    enum class CommandType : uint16_t { SendRequest = 0x0101, ReceiveAccept = 0x0202, TransferComplete = 0x0204 };

private:
    uint16_t m_commandType;
    QString m_fileName;
    uint32_t m_totalFileSize;

    OscarFileTransferPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

public:
    CommandType Type() const { return static_cast<CommandType>(m_commandType); }
    QString FileName() const { return m_fileName; }
    uint32_t TotalFileSize() const { return m_totalFileSize; }

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

    bool PacketHeaderIsComplete() const override { return true; }
    int ParsedBytesCount() const override { return 256; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
