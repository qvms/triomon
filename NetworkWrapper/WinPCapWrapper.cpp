#include "WinPCapWrapper.hpp"
#include <QDebug>
#include <cstring>
#include <arpa/inet.h>

namespace NetworkWrapper {

WinPCapWrapper::WinPCapWrapper() {
    WinPCapNative::init();
    memset(errbuf, 0, sizeof(errbuf));
}

WinPCapWrapper::~WinPCapWrapper() {
    Close();
}

QList<Device> WinPCapWrapper::FindAllDevs() {
    WinPCapNative::init();
    QList<Device> deviceList;
    WinPCapNative::pcap_if* pcap_if_list = nullptr;
    char errbuf[WinPCapNative::PCAP_ERRBUF_SIZE];
    
    if (WinPCapNative::pcap_findalldevs && WinPCapNative::pcap_findalldevs(&pcap_if_list, errbuf) != -1) {
        WinPCapNative::pcap_if* current = pcap_if_list;
        while (current != nullptr) {
            Device device;
            device.Name = QString::fromUtf8(current->name);
            device.Description = current->description ? QString::fromUtf8(current->description) : "";
            
            WinPCapNative::pcap_addr* addr = reinterpret_cast<WinPCapNative::pcap_addr*>(current->addresses);
            if (addr != nullptr) {
                if (addr->addr != nullptr && addr->addr->sa_family == AF_INET) {
                    struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(addr->addr);
                    char ipStr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(ipv4->sin_addr), ipStr, INET_ADDRSTRLEN);
                    device.Address = QString::fromUtf8(ipStr);
                }
                if (addr->netmask != nullptr && addr->netmask->sa_family == AF_INET) {
                    struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(addr->netmask);
                    char ipStr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(ipv4->sin_addr), ipStr, INET_ADDRSTRLEN);
                    device.Netmask = QString::fromUtf8(ipStr);
                }
            }
            deviceList.append(device);
            current = current->next;
        }
        if (WinPCapNative::pcap_freealldevs) {
            WinPCapNative::pcap_freealldevs(pcap_if_list);
        }
    }
    return deviceList;
}

bool WinPCapWrapper::Open(const QString& source, int snaplen, int flags, int read_timeout) {
    if (IsOpen()) {
        throw AlreadyOpenException();
    }
    
    if (!WinPCapNative::pcap_open_live) return false;

    pcap_t = WinPCapNative::pcap_open_live(source.toUtf8().constData(), snaplen, flags, read_timeout, errbuf);
    
    if (pcap_t != nullptr) {
        m_isopen = true;
        m_attachedDevice = source;
        GetDatalink();
        return true;
    }
    return false;
}

void WinPCapWrapper::GetDatalink() {
    if (pcap_t && WinPCapNative::pcap_datalink) {
        datalink = WinPCapNative::pcap_datalink(pcap_t);
    }
}

bool WinPCapWrapper::SetMinToCopy(int size) {
    if (pcap_t && WinPCapNative::pcap_setmintocopy) {
        return WinPCapNative::pcap_setmintocopy(pcap_t, size) == 0;
    }
    return false;
}

bool WinPCapWrapper::SetKernelBuffer(int bytes) {
    if (pcap_t && WinPCapNative::pcap_setbuff) {
        return WinPCapNative::pcap_setbuff(pcap_t, bytes) == 0;
    }
    return false;
}

void WinPCapWrapper::ListenThreadWorker::run() {
    wrapper->ReadNextLoop();
}

WinPCapNative::PCAP_NEXT_EX_STATE WinPCapWrapper::ReadNextInternal(PcapHeader*& packetHeader, QByteArray& arr) {
    if (!pcap_t || !WinPCapNative::pcap_next_ex) return WinPCapNative::PCAP_NEXT_EX_STATE::ERROR;
    
    WinPCapNative::pcap_pkthdr* hdr = nullptr;
    const uint8_t* data = nullptr;
    
    int result = WinPCapNative::pcap_next_ex(pcap_t, &hdr, &data);
    
    if (result == 1) { // SUCCESS
        packetHeader = new PcapHeader(*hdr);
        arr = QByteArray(reinterpret_cast<const char*>(data), hdr->caplen);
        return WinPCapNative::PCAP_NEXT_EX_STATE::SUCCESS;
    } else if (result == 0) {
        return WinPCapNative::PCAP_NEXT_EX_STATE::TIMEOUT;
    } else if (result == -1) {
        return WinPCapNative::PCAP_NEXT_EX_STATE::ERROR;
    } else if (result == -2) {
        return WinPCapNative::PCAP_NEXT_EX_STATE::END_OF_FILE;
    }
    return WinPCapNative::PCAP_NEXT_EX_STATE::UNKNOWN;
}

void WinPCapWrapper::ReadNextLoop() {
    while (m_islistening && !disposed) {
        PcapHeader* header = nullptr;
        QByteArray arr;
        
        WinPCapNative::PCAP_NEXT_EX_STATE state = ReadNextInternal(header, arr);
        
        if (state == WinPCapNative::PCAP_NEXT_EX_STATE::SUCCESS) {
            if (PacketArrival) {
                PacketArrival(this, header, arr);
            }
            delete header;
        } else if (state == WinPCapNative::PCAP_NEXT_EX_STATE::ERROR || state == WinPCapNative::PCAP_NEXT_EX_STATE::END_OF_FILE) {
            break;
        }
    }
}

void WinPCapWrapper::StartListen() {
    if (listenThread != nullptr) {
        m_islistening = false;
        listenThread->quit();
        listenThread->wait();
        delete listenThread;
        listenThread = nullptr;
    }

    m_islistening = true;
    listenThread = new ListenThreadWorker(this);
    listenThread->start();
}

void WinPCapWrapper::StopListen() {
    m_islistening = false;
    if (listenThread != nullptr) {
        listenThread->quit();
        listenThread->wait();
        delete listenThread;
        listenThread = nullptr;
    }
}

void WinPCapWrapper::Close() {
    if (IsListening()) {
        StopListen();
    }
    
    m_isopen = false;
    m_attachedDevice.clear();
    
    if (pcap_t != nullptr && WinPCapNative::pcap_close) {
        WinPCapNative::pcap_close(pcap_t);
        pcap_t = nullptr;
    }
    disposed = true;
}

} // namespace NetworkWrapper
