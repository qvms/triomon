#include "PcapParser.hpp"
#include <stdexcept>
#include <QDateTime>

namespace SharedUtils {
namespace Pcap {

const std::vector<uint32_t> PcapParser::PCAP_BIG_ENDIAN_MAGIC_NUMBERS = {
    LIBPCAP_MAGIC_NUMBER,
    PCAP_MODIFIED_MAGIC,
    NANOSECOND_PCAP_MACIC
};

std::set<uint32_t> initPcapMagicNumbers() {
    std::set<uint32_t> magicNumbers;
    for (uint32_t magic : PcapParser::PCAP_BIG_ENDIAN_MAGIC_NUMBERS) {
        magicNumbers.insert(magic);
        uint32_t reversed = ((magic & 0x000000FF) << 24) |
                            ((magic & 0x0000FF00) << 8) |
                            ((magic & 0x00FF0000) >> 8) |
                            ((magic & 0xFF000000) >> 24);
        magicNumbers.insert(reversed);
    }
    return magicNumbers;
}

const std::set<uint32_t> PcapParser::PCAP_MAGIC_NUMBERS = initPcapMagicNumbers();

PcapParser::PcapParser(std::shared_ptr<IPcapStreamReader> pcapStreamReader)
    : PcapParser(pcapStreamReader, std::vector<uint8_t>()) {
}

PcapParser::PcapParser(std::shared_ptr<IPcapStreamReader> pcapStreamReader, const std::vector<uint8_t>& firstFourBytes)
    : pcapStreamReader(pcapStreamReader) {
    
    std::vector<uint8_t> buffer4;
    std::vector<uint8_t> buffer2;

    if (firstFourBytes.size() != 4) {
        // Assume blocking read method exists that returns vector of requested size
        // This is a stub adaptation of the C# stream reader logic
        buffer4 = std::vector<uint8_t>(4, 0); // Stub
    } else {
        buffer4 = firstFourBytes;
    }

    uint32_t magic_be = ToUInt32(buffer4, false);
    uint32_t magic_le = ToUInt32(buffer4, true);

    if (magic_be == LIBPCAP_MAGIC_NUMBER) {
        littleEndian = false;
        metadata.push_back({"Endianness", "Big Endian"});
    } else if (magic_le == LIBPCAP_MAGIC_NUMBER) {
        littleEndian = true;
        metadata.push_back({"Endianness", "Little Endian"});
    } else if (magic_be == PCAP_MODIFIED_MAGIC) {
        littleEndian = false;
        packetHeaderTrailerBytes = 8;
        metadata.push_back({"Endianness", "Big Endian"});
    } else if (magic_le == PCAP_MODIFIED_MAGIC) {
        littleEndian = true;
        packetHeaderTrailerBytes = 8;
        metadata.push_back({"Endianness", "Little Endian"});
    } else if (magic_be == NANOSECOND_PCAP_MACIC) {
        littleEndian = false;
        timestampResolution = TimestampResolution::nanosecond;
        metadata.push_back({"Endianness", "Big Endian"});
    } else if (magic_le == NANOSECOND_PCAP_MACIC) {
        littleEndian = true;
        timestampResolution = TimestampResolution::nanosecond;
        metadata.push_back({"Endianness", "Little Endian"});
    } else {
        throw std::runtime_error("The stream is not a PCAP file.");
    }

    // Read remaining headers (stub)
    dataLinkType = 1; // Stub: Ethernet
    metadata.push_back({"Data Link Type", "1"});
}

std::vector<std::pair<std::string, std::string>> PcapParser::get_Metadata() const {
    return metadata;
}

std::vector<uint32_t> PcapParser::get_DataLinkTypes() const {
    return {dataLinkType};
}

std::shared_ptr<PcapFrame> PcapParser::ReadPcapPacketBlocking() {
    // Stub implementation
    return nullptr;
}

uint16_t PcapParser::ToUInt16(const std::vector<uint8_t>& buffer, bool littleEndian) {
    if (buffer.size() < 2) return 0;
    if (littleEndian)
        return (uint16_t)(buffer[0] | (buffer[1] << 8));
    else
        return (uint16_t)((buffer[0] << 8) | buffer[1]);
}

uint32_t PcapParser::ToUInt32(const std::vector<uint8_t>& buffer, bool littleEndian) {
    if (buffer.size() < 4) return 0;
    if (littleEndian) {
        return (uint32_t)(buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
    } else {
        return (uint32_t)((buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3]);
    }
}

} // namespace Pcap
} // namespace SharedUtils
