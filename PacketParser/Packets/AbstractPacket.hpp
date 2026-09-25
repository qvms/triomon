#pragma once
#include <QString>
#include <QHash>
#include <vector>
#include <memory>
#include "IPacket.hpp"
#include "../Frame.hpp"

namespace PacketParser {
namespace Packets {

class AbstractPacket : public virtual IPacket {
private:
    const Frame* m_parentFrame;
    QString m_packetTypeDescription;
    int m_packetStartIndex;
    int m_packetEndIndex;
    QHash<QString, QString> m_attributes;

public:
    AbstractPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, const QString& packetTypeDescription);
    virtual ~AbstractPacket() = default;

    const Frame* ParentFrame() const override { return m_parentFrame; }
    const QString& PacketTypeDescription() const { return m_packetTypeDescription; }
    
    int PacketStartIndex() const override { return m_packetStartIndex; }
    void setPacketStartIndex(int value) { m_packetStartIndex = value; }

    int PacketEndIndex() const { return m_packetEndIndex; }
    void setPacketEndIndex(int value) {
        if (value >= m_packetStartIndex && static_cast<size_t>(value) < m_parentFrame->DataLength()) {
            m_packetEndIndex = value;
        }
    }

    int PacketLength() const { return m_packetEndIndex - m_packetStartIndex + 1; }
    int PacketByteCount() const { return PacketLength(); }
    const QHash<QString, QString>& Attributes() const { return m_attributes; }
    void addAttribute(const QString& key, const QString& value) { m_attributes.insert(key, value); }

    std::vector<uint8_t> GetPacketData() const;

    virtual std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) = 0;
};

} // namespace Packets
} // namespace PacketParser
    

        
