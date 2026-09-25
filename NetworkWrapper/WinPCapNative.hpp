#include <sys/socket.h>
#pragma once

#include <QLibrary>
#include <QString>
#include <stdint.h>
#include <stddef.h>

namespace NetworkWrapper {

class WinPCapNative {
public:
    static const int INFINITE = -1;
    static const char* PCAP_NAME_PREFIX;
    static const uint32_t PCAP_IF_LOOPBACK = 0x00000001;
    static const int MAX_ADAPTER_NAME_LENGTH = 256;
    static const int MAX_ADAPTER_DESCRIPTION_LENGTH = 128;
    static const int MAX_PACKET_SIZE = 65536;
    static const int PCAP_ERRBUF_SIZE = 256;
    static const int PCAP_BUF_SIZE = 1024;
    static const int MODE_CAPT = 0;
    static const int MODE_STAT = 1;
    static const char* PCAP_SRC_IF_STRING;
    static const int PCAP_SRC_FILE = 2;
    static const int PCAP_SRC_IFLOCAL = 3;
    static const int PCAP_SRC_IFREMOTE = 4;
    static const int PCAP_OPENFLAG_PROMISCUOUS = 1;

    struct timeval {
        uint32_t tv_sec;
        uint32_t tv_usec;
    };

    struct pcap_pkthdr {
        timeval ts;
        int caplen;
        int len;
    };

    struct pcap_stat {
        uint32_t ps_recv;
        uint32_t ps_drop;
        uint32_t ps_ifdrop;
        uint32_t bs_capt;
    };

    struct pcap_if {
        pcap_if* next;
        char* name;
        char* description;
        struct pcap_addr* addresses;
        uint32_t flags;
    };

    struct pcap_addr {
        pcap_addr* next;
        struct ::sockaddr* addr;
        struct ::sockaddr* netmask;
        struct ::sockaddr* broadaddr;
        struct ::sockaddr* dstaddr;
    };

    struct sockaddr {
        short family;
        uint16_t port;
        uint8_t addr[4];
        uint8_t zero[8];
    };

    enum class PCAP_NEXT_EX_STATE {
        SUCCESS = 1,
        TIMEOUT = 0,
        ERROR = -1,
        END_OF_FILE = -2,
        UNKNOWN = -3,
    };

    typedef void (*dispatcher_handler)(uint8_t* user, const struct pcap_pkthdr* header, const uint8_t* pkt_data);

    // QLibrary function pointers
    typedef void (*pcap_close_t)(void* p);
    typedef int (*pcap_findalldevs_t)(pcap_if** alldevsp, char* errbuf);
    typedef void (*pcap_freealldevs_t)(pcap_if* alldevsp);
    typedef void* (*pcap_open_live_t)(const char* device, int snaplen, int promisc, int to_ms, char* errbuf);
    typedef int (*pcap_setmintocopy_t)(void* p, int size);
    typedef int (*pcap_datalink_t)(void* p);
    typedef int (*pcap_next_ex_t)(void* p, pcap_pkthdr** pkt_header, const uint8_t** pkt_data);
    typedef int (*pcap_setbuff_t)(void* p, int dim);

    static pcap_close_t pcap_close;
    static pcap_findalldevs_t pcap_findalldevs;
    static pcap_freealldevs_t pcap_freealldevs;
    static pcap_open_live_t pcap_open_live;
    static pcap_setmintocopy_t pcap_setmintocopy;
    static pcap_datalink_t pcap_datalink;
    static pcap_next_ex_t pcap_next_ex;
    static pcap_setbuff_t pcap_setbuff;

    static void init();
};

} // namespace NetworkWrapper
