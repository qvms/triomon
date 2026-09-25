#include "WinPCapNative.hpp"
#include <QDebug>

namespace NetworkWrapper {

const char* WinPCapNative::PCAP_NAME_PREFIX = "\\Device\\NPF_";
const char* WinPCapNative::PCAP_SRC_IF_STRING = "rpcap://";

WinPCapNative::pcap_close_t WinPCapNative::pcap_close = nullptr;
WinPCapNative::pcap_findalldevs_t WinPCapNative::pcap_findalldevs = nullptr;
WinPCapNative::pcap_freealldevs_t WinPCapNative::pcap_freealldevs = nullptr;
WinPCapNative::pcap_open_live_t WinPCapNative::pcap_open_live = nullptr;
WinPCapNative::pcap_setmintocopy_t WinPCapNative::pcap_setmintocopy = nullptr;
WinPCapNative::pcap_datalink_t WinPCapNative::pcap_datalink = nullptr;
WinPCapNative::pcap_next_ex_t WinPCapNative::pcap_next_ex = nullptr;
WinPCapNative::pcap_setbuff_t WinPCapNative::pcap_setbuff = nullptr;

void WinPCapNative::init() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    QLibrary pcapLib("pcap");
    if (!pcapLib.load()) {
        pcapLib.setFileName("wpcap"); // Try Windows pcap name fallback
        if (!pcapLib.load()) {
            qWarning() << "Failed to load pcap library";
            return;
        }
    }

    pcap_close = (pcap_close_t)pcapLib.resolve("pcap_close");
    pcap_findalldevs = (pcap_findalldevs_t)pcapLib.resolve("pcap_findalldevs");
    pcap_freealldevs = (pcap_freealldevs_t)pcapLib.resolve("pcap_freealldevs");
    pcap_open_live = (pcap_open_live_t)pcapLib.resolve("pcap_open_live");
    pcap_setmintocopy = (pcap_setmintocopy_t)pcapLib.resolve("pcap_setmintocopy");
    pcap_datalink = (pcap_datalink_t)pcapLib.resolve("pcap_datalink");
    pcap_next_ex = (pcap_next_ex_t)pcapLib.resolve("pcap_next_ex");
    pcap_setbuff = (pcap_setbuff_t)pcapLib.resolve("pcap_setbuff");
}

} // namespace NetworkWrapper
