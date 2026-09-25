#ifndef SHAREDUTILS_PCAP_PCAPPACKET_HPP
#define SHAREDUTILS_PCAP_PCAPPACKET_HPP

#include <QDateTime>
#include <QByteArray>

namespace SharedUtils {
namespace Pcap {

class PcapPacket {
public:
    PcapPacket(const QDateTime& timestamp, const QByteArray& data);

    QDateTime timestamp() const;
    QByteArray data() const;

private:
    QDateTime m_timestamp;
    QByteArray m_data;
};

} // namespace Pcap
} // namespace SharedUtils

#endif // SHAREDUTILS_PCAP_PCAPPACKET_HPP
