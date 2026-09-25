#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>

namespace NetworkMiner {
namespace PacketStream {

class ReceivePcapOverTcp : public QDialog {
    Q_OBJECT

public:
    explicit ReceivePcapOverTcp(QWidget* parent = nullptr);
    ~ReceivePcapOverTcp() override = default;

private slots:
    void onStartReceiving();

private:
    QSpinBox* m_portNumberSelectorIncoming;
    QSpinBox* m_timoutSelector;
    QCheckBox* m_useSslCheckBox;
    QRadioButton* m_incomingTcpRadioButton;
    QPushButton* m_startReceivingButton;
};

} // namespace PacketStream
} // namespace NetworkMiner
