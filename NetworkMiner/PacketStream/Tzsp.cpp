#include "Tzsp.hpp"
#include <QMessageBox>

namespace NetworkMiner {
namespace PacketStream {

Tzsp::Tzsp(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Receive TZSP");
    resize(250, 150);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_udpPortSelector = new QSpinBox(this);
    m_udpPortSelector->setRange(1, 65535);
    m_udpPortSelector->setValue(37008);
    layout->addWidget(new QLabel("UDP Port:"));
    layout->addWidget(m_udpPortSelector);

    m_startReceivingButton = new QPushButton("Start Receiving", this);
    layout->addWidget(m_startReceivingButton);

    connect(m_startReceivingButton, &QPushButton::clicked, this, &Tzsp::onStartReceiving);
}

void Tzsp::onStartReceiving() {
    QMessageBox::information(this, "Started", "Started listening for TZSP (mock)");
    accept();
}

} // namespace PacketStream
} // namespace NetworkMiner
