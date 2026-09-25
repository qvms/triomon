#include "PcapPacket.hpp"

namespace SharedUtils {
namespace Pcap {

PcapPacket::PcapPacket(const QDateTime& timestamp, const QByteArray& data)
    : m_timestamp(timestamp),
      m_data(data)
{
}

QDateTime PcapPacket::timestamp() const
{
    return m_timestamp;
}

QByteArray PcapPacket::data() const
{
    return m_data;
}

} // namespace Pcap
} // namespace SharedUtils
