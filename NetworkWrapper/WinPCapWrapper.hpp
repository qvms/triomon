#pragma once

#include "WinPCapNative.hpp"
#include <QString>
#include <QList>
#include <QDateTime>
#include <QThread>
#include <QByteArray>
#include <functional>
#include <exception>

namespace NetworkWrapper {

class AlreadyOpenException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Device attached to object already open. Close first before reopening";
    }
};

class Device {
public:
    QString Name;
    QString Description;
    QString Address;
    QString Netmask;

    Device() = default;
    Device(const QString& name, const QString& description, const QString& address, const QString& netmask)
        : Name(name), Description(description), Address(address), Netmask(netmask) {}
};

class PcapHeader {
public:
    WinPCapNative::pcap_pkthdr _Pkhdr;

    PcapHeader() {
        _Pkhdr.ts.tv_sec = 0;
        _Pkhdr.ts.tv_usec = 0;
        _Pkhdr.caplen = 0;
        _Pkhdr.len = 0;
    }

    PcapHeader(WinPCapNative::timeval tv, int plength, int clength) {
        _Pkhdr.ts = tv;
        _Pkhdr.caplen = plength;
        _Pkhdr.len = clength;
    }

    PcapHeader(const WinPCapNative::pcap_pkthdr& value) : _Pkhdr(value) {}

    WinPCapNative::timeval Timeval() const { return _Pkhdr.ts; }
    void SetTimeval(WinPCapNative::timeval value) { _Pkhdr.ts = value; }

    int Seconds() const { return _Pkhdr.ts.tv_sec; }
    void SetSeconds(int value) { _Pkhdr.ts.tv_sec = value; }

    int MicroSeconds() const { return _Pkhdr.ts.tv_usec; }
    void SetMicroSeconds(int value) { _Pkhdr.ts.tv_usec = value; }

    QDateTime TimeStamp() const {
        return QDateTime::fromSecsSinceEpoch(_Pkhdr.ts.tv_sec).addMSecs(_Pkhdr.ts.tv_usec / 1000);
    }

    int PacketLength() const { return _Pkhdr.len; }
    void SetPacketLength(int value) { _Pkhdr.len = value; }

    int CaptureLength() const { return _Pkhdr.caplen; }
    void SetCaptureLength(int value) { _Pkhdr.caplen = value; }
};

class WinPCapWrapper : public WinPCapNative {
private:
    class ListenThreadWorker : public QThread {
    private:
        WinPCapWrapper* wrapper;
    public:
        ListenThreadWorker(WinPCapWrapper* wrapper) : wrapper(wrapper) {}
        void run() override;
    };

    ListenThreadWorker* listenThread = nullptr;
    bool disposed = false;
    int datalink = 0;
    QString fname = "";
    int maxb = 0;
    int maxp = 0;
    bool m_islistening = false;
    bool m_isopen = false;
    QString m_attachedDevice;
    void* pcap_t = nullptr;
    char errbuf[WinPCapNative::PCAP_ERRBUF_SIZE];

    void ReadNextLoop();

    WinPCapNative::PCAP_NEXT_EX_STATE ReadNextInternal(PcapHeader*& packetHeader, QByteArray& arr);

public:
    using PacketArrivalEventHandler = std::function<void(WinPCapWrapper*, PcapHeader*, const QByteArray&)>;
    using EndCaptureEventHandler = std::function<void(WinPCapWrapper*)>;

    PacketArrivalEventHandler PacketArrival;
    EndCaptureEventHandler EndCapture;

    WinPCapWrapper();
    virtual ~WinPCapWrapper();

    QString AttachedDevice() const { return m_attachedDevice; }
    int DataLink() const { return datalink; }
    QString LastError() const { return QString::fromUtf8(errbuf); }
    bool IsListening() const { return m_islistening; }
    bool IsOpen() const { return m_isopen; }

    static QList<Device> FindAllDevs();

    bool Open(const QString& source, int snaplen, int flags, int read_timeout);
    void GetDatalink();
    bool SetMinToCopy(int size);
    bool SetKernelBuffer(int bytes);
    
    void StartListen();
    void StopListen();
    void Close();
};

} // namespace NetworkWrapper
