#include "EtlParser.hpp"
#include <stdexcept>

namespace SharedUtils {
namespace Pcap {

EtlParser::EtlParser() {}

std::shared_ptr<PacketParser::PcapFrame> EtlParser::ReadPcapPacketBlocking() {
    throw std::runtime_error("NotImplementedException: EtlParser not supported on non-Windows");
}

void EtlParser::Dispose() { }

} // namespace Pcap
} // namespace SharedUtils
