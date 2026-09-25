#include "PacketFactory.hpp"
#include "Ethernet2Packet.hpp"
#include "IEEE_802_11Packet.hpp"
#include "IEEE_802_11RadiotapPacket.hpp"
#include "IPv4Packet.hpp"
#include "IPv6Packet.hpp"
#include "CiscoHdlcPacket.hpp"
#include "PrismCaptureHeaderPacket.hpp"
#include "PpiPacket.hpp"
#include "PointToPointPacket.hpp"
#include "RawPacket.hpp"

#if __has_include("LinuxCookedCapture.hpp")
#include "LinuxCookedCapture.hpp"
#endif
#if __has_include("LinuxCookedCapture2.hpp")
#include "LinuxCookedCapture2.hpp"
#endif
#if __has_include("NullLoopbackPacket.hpp")
#include "NullLoopbackPacket.hpp"
#endif
#if __has_include("ErfFrame.hpp")
#include "ErfFrame.hpp"
#endif
#if __has_include("TzspPacket.hpp")
#include "TzspPacket.hpp"
#endif

namespace PacketParser {
namespace Packets {

bool PacketFactory::TryGetPacket(AbstractPacket*& packet, DataLinkTypeEnum dataLinkType, const Frame* parentFrame, int startIndex, int endIndex) {
    packet = nullptr;
    try {
        if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_ETHERNET) {
            packet = new Ethernet2Packet(parentFrame, startIndex, endIndex);
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_IEEE_802_11 || dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_IEEE_802_11_WLAN_AVS) {
            packet = new IEEE_802_11Packet(parentFrame, startIndex, endIndex);
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_IEEE_802_11_WLAN_RADIOTAP) {
            packet = new IEEE_802_11RadiotapPacket(parentFrame, startIndex, endIndex);
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_RAW_IP || 
                   dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_RAW_IP_2 ||
                   dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_RAW_IP_3 ||
                   dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_RAW_IP4) {
            if (!IPv4Packet::TryParse(parentFrame, startIndex, endIndex, packet)) {
                packet = nullptr;
            }
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_RAW_IP6) {
            packet = new IPv6Packet(parentFrame, startIndex, endIndex);
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_CHDLC) {
            packet = new CiscoHdlcPacket(parentFrame, startIndex, endIndex);
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_PRISM_HEADER) {
            packet = new PrismCaptureHeaderPacket(parentFrame, startIndex, endIndex);
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_PPI) {
            packet = new PpiPacket(parentFrame, startIndex, endIndex);
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_PPP || dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_PPP_2) {
            packet = new PointToPointPacket(parentFrame, startIndex, endIndex);
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_SLL) {
#if __has_include("LinuxCookedCapture.hpp")
            packet = new LinuxCookedCapture(parentFrame, startIndex, endIndex);
#endif
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_NULL) {
#if __has_include("NullLoopbackPacket.hpp")
            packet = new NullLoopbackPacket(parentFrame, startIndex, endIndex);
#endif
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_ERF) {
#if __has_include("ErfFrame.hpp")
            packet = new ErfFrame(parentFrame, startIndex, endIndex);
#endif
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_LINUX_SLL2) {
#if __has_include("LinuxCookedCapture2.hpp")
            packet = new LinuxCookedCapture2(parentFrame, startIndex, endIndex);
#endif
        } else if (dataLinkType == DataLinkTypeEnum::WTAP_ENCAP_TZSP) {
#if __has_include("TzspPacket.hpp")
            packet = new TzspPacket(parentFrame, startIndex, endIndex);
#endif
        }

        if (!packet) {
            packet = new RawPacket(parentFrame, startIndex, endIndex);
            return false;
        }
        return true;
    } catch (...) {
        packet = new RawPacket(parentFrame, startIndex, endIndex);
        return false;
    }
}

} // namespace Packets
} // namespace PacketParser
