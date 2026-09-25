#pragma once
#include <cstdint>

namespace PacketParser {

enum class DataLinkTypeEnum : uint32_t {
    WTAP_ENCAP_NULL = 0,
    WTAP_ENCAP_ETHERNET = 1,
    WTAP_ENCAP_PRISM_HEADER = 119,
    WTAP_ENCAP_IEEE_802_11 = 105,
    WTAP_ENCAP_IEEE_802_11_WLAN_AVS = 163,
    WTAP_ENCAP_IEEE_802_11_WLAN_RADIOTAP = 127,
    WTAP_ENCAP_RAW_IP = 101,
    WTAP_ENCAP_RAW_IP_2 = 12,
    WTAP_ENCAP_RAW_IP_3 = 14,
    WTAP_ENCAP_RAW_IP4 = 228,
    WTAP_ENCAP_RAW_IP6 = 229,
    WTAP_ENCAP_CHDLC = 104,
    WTAP_ENCAP_SLL = 113,
    WTAP_ENCAP_PPI = 192,
    WTAP_ENCAP_PPP = 9,
    WTAP_ENCAP_PPP_2 = 50,
    WTAP_ENCAP_ERF = 197,
    WTAP_ENCAP_LINUX_SLL2 = 276,
    WTAP_ENCAP_TZSP = 0xFFFFFFFF // Not standard pcap DLT, custom maybe? TZSP is usually UDP
};

} // namespace PacketParser
