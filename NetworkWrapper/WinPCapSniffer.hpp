#pragma once

#include "ISniffer.hpp"
#include "PacketReceivedEventArgs.hpp"
#include "WinPCapAdapter.hpp"
#include "WinPCapWrapper.hpp"
#include <memory>
#include <functional>
#include <QObject>

namespace NetworkWrapper {

class WinPCapSniffer : public QObject, public ISniffer {
    Q_OBJECT
public:
    enum class DataLinkType {
        WTAP_ENCAP_UNKNOWN = 0,
        WTAP_ENCAP_ETHERNET = 1,
        // ... (truncated list from C#)
        WTAP_ENCAP_IEEE_802_11 = 105,
        WTAP_ENCAP_IEEE_802_11_WLAN_RADIOTAP = 127,
        WTAP_ENCAP_RAW_IP = 14,
        WTAP_ENCAP_RAW_IP_2 = 12,
        WTAP_ENCAP_RAW_IP_3 = 14,
    };

private:
    std::shared_ptr<WinPCapWrapper> wpcap;
    int nPacketsReceived;
    PacketReceivedEventArgs::PacketTypes basePacketType;

    void ReceivePacketListener(WinPCapWrapper* sender, PcapHeader* ph, const QByteArray& data);

public:
    using PacketReceivedHandler = std::function<void(WinPCapSniffer*, const PacketReceivedEventArgs&)>;
    static PacketReceivedHandler PacketReceived;

    WinPCapSniffer(const WinPCapAdapter& adapter, QObject* parent = nullptr);
    virtual ~WinPCapSniffer();

    PacketReceivedEventArgs::PacketTypes GetBasePacketType() const override { return basePacketType; }
    void StartSniffing() override;
    void StopSniffing() override;
};

} // namespace NetworkWrapper
