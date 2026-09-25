#pragma once

#include "IPcapParser.hpp"
#include "IPcapStreamReader.hpp"
#include "PcapFrame.hpp"
#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>
#include <cstdint>

namespace SharedUtils {
namespace Pcap {

class PcapParser : public IPcapParser {
public:
    enum class TimestampResolution { microsecond, nanosecond };

    static const uint32_t LIBPCAP_MAGIC_NUMBER = 0xa1b2c3d4;
    static const uint32_t PCAP_MODIFIED_MAGIC = 0xa1b2cd34;
    static const uint32_t NANOSECOND_PCAP_MACIC = 0xa1b23c4d;

    static const std::vector<uint32_t> PCAP_BIG_ENDIAN_MAGIC_NUMBERS;
    static const std::set<uint32_t> PCAP_MAGIC_NUMBERS;

private:
    uint32_t dataLinkType;
    int packetHeaderTrailerBytes = 0;
    std::shared_ptr<IPcapStreamReader> pcapStreamReader;
    TimestampResolution timestampResolution = TimestampResolution::microsecond;
    bool littleEndian;
    std::vector<std::pair<std::string, std::string>> metadata;

    uint16_t ToUInt16(const std::vector<uint8_t>& buffer, bool littleEndian);
    uint32_t ToUInt32(const std::vector<uint8_t>& buffer, bool littleEndian);

public:
    PcapParser(std::shared_ptr<IPcapStreamReader> pcapStreamReader);
    PcapParser(std::shared_ptr<IPcapStreamReader> pcapStreamReader, const std::vector<uint8_t>& firstFourBytes);

    std::vector<std::pair<std::string, std::string>> get_Metadata() const;
    std::vector<uint32_t> get_DataLinkTypes() const;

    std::shared_ptr<PcapFrame> ReadPcapPacketBlocking();
};

} // namespace Pcap
} // namespace SharedUtils
