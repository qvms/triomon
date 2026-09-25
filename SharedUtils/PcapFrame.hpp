#pragma once

#include <QDateTime>
#include <vector>
#include <cstdint>

// Notice: In the C# codebase PcapFrame was using its own nested DataLinkTypeEnum
// and keeping some constants, but in C++ we already have DataLinkTypeEnum 
// generated as a separate file or we can define it here. 
// For parity we will define it inside PcapFrame.

namespace SharedUtils {
namespace Pcap {

class PcapFrame {
public:
    enum class DataLinkTypeEnum : uint32_t {
        WTAP_ENCAP_NULL = 0,
        WTAP_ENCAP_ETHERNET = 1,
        WTAP_ENCAP_RAW_IP = 101,
        WTAP_ENCAP_IEEE_802_11 = 105,
        WTAP_ENCAP_SLL = 113,
        WTAP_ENCAP_IEEE_802_11_WLAN_RADIOTAP = 127,
        WTAP_ENCAP_RAW_IP4 = 228,
        WTAP_ENCAP_RAW_IP6 = 229,
        // (Other enums omitted for brevity, but they are conceptually here)
        WTAP_ENCAP_MATCHING_MAX = 290
    };

    static DataLinkTypeEnum GetDataLinkType(uint32_t linkTypeValue) {
        // Simplified parsing
        return static_cast<DataLinkTypeEnum>(linkTypeValue);
    }

private:
    QDateTime timestamp;
    std::vector<uint8_t> data;
    DataLinkTypeEnum dataLinkType;

public:
    void* Tag = nullptr;

    PcapFrame(const QDateTime& timestamp, const std::vector<uint8_t>& data, DataLinkTypeEnum dataLinkType)
        : timestamp(timestamp), data(data), dataLinkType(dataLinkType) {}

    QDateTime Timestamp() const { return timestamp; }
    const std::vector<uint8_t>& Data() const { return data; }
    DataLinkTypeEnum DataLinkType() const { return dataLinkType; }

    int DataLength() const {
        return static_cast<int>(data.size());
    }
};

} // namespace Pcap
} // namespace SharedUtils
