#pragma once

#include "PcapFrame.hpp" // Placeholder

namespace SharedUtils {
namespace Pcap {

class EtlParser {
private:
    EtlParser(); // Prevent instantiation as per #else block in C# (assuming non-Windows/no ETW support in Linux port)

public:
    std::shared_ptr<PacketParser::PcapFrame> ReadPcapPacketBlocking();
    // Task<PcapFrame> ReadPcapPacketAsync(CancellationToken cancellationToken);
    void Dispose();
};

} // namespace Pcap
} // namespace SharedUtils
