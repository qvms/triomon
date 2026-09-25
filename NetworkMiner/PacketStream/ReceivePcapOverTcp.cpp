#include "ReceivePcapOverTcp.hpp"
#include <QMessageBox>

namespace NetworkMiner {
namespace PacketStream {

ReceivePcapOverTcp::ReceivePcapOverTcp(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Receive PCAP over TCP");
    resize(300, 200);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_incomingTcpRadioButton = new QRadioButton("Listen for incoming connections", this);
    m_incomingTcpRadioButton->setChecked(true);
    layout->addWidget(m_incomingTcpRadioButton);

    m_portNumberSelectorIncoming = new QSpinBox(this);
    m_portNumberSelectorIncoming->setRange(1, 65535);
    m_portNumberSelectorIncoming->setValue(57012);
    layout->addWidget(new QLabel("Port:"));
    layout->addWidget(m_portNumberSelectorIncoming);

    m_timoutSelector = new QSpinBox(this);
    m_timoutSelector->setRange(1, 3600);
    m_timoutSelector->setValue(60);
    layout->addWidget(new QLabel("Idle Timeout (s):"));
    layout->addWidget(m_timoutSelector);

    m_useSslCheckBox = new QCheckBox("Use SSL/TLS", this);
    layout->addWidget(m_useSslCheckBox);

    m_startReceivingButton = new QPushButton("Start Receiving", this);
    layout->addWidget(m_startReceivingButton);

    connect(m_startReceivingButton, &QPushButton::clicked, this, &ReceivePcapOverTcp::onStartReceiving);
}

void ReceivePcapOverTcp::onStartReceiving() {
    QMessageBox::information(this, "Started", "Started listening for PCAP-over-IP (mock)");
    accept();
}

} // namespace PacketStream
} // namespace NetworkMiner
