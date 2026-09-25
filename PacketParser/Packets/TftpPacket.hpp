#pragma once
#include "AbstractPacket.hpp"
#include <QString>
#include <vector>
#include <map>

namespace PacketParser {
namespace Packets {

class TftpPacket : public AbstractPacket {
public:
    static const uint16_t DefaultUdpPortNumber = 69;

    enum class OpCodes : uint16_t {
        ReadRequest = 0x01,
        WriteRequest = 0x02,
        Data = 0x03,
        Acknowledgment = 0x04,
        Error = 0x05,
        OptionAcknowledgment = 0x06
    };

    enum class Modes { netascii, octet, mail };

private:
    uint16_t m_opCode;
    QString m_filename;
    Modes m_mode;
    uint16_t m_dataBlockNumber;
    std::vector<uint8_t> m_dataBlock;
    uint16_t m_blksize;
    std::map<QString, QString> m_rfc2347OptionList;

public:
    TftpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint16_t blksize = 512);

    OpCodes OpCode() const { return static_cast<OpCodes>(m_opCode); }
    Modes Mode() const { return m_mode; }
    QString Filename() const { return m_filename; }
    const std::vector<uint8_t>& DataBlock() const { return m_dataBlock; }
    uint16_t DataBlockNumber() const { return m_dataBlockNumber; }
    bool DataBlockIsLast() const { return m_dataBlock.size() < m_blksize; }
    uint16_t Blksize() const { return m_blksize; }

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
