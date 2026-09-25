#pragma once

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

namespace NetworkMiner {
namespace PacketStream {

class Tzsp : public QDialog {
    Q_OBJECT

public:
    explicit Tzsp(QWidget* parent = nullptr);
    ~Tzsp() override = default;

private slots:
    void onStartReceiving();

private:
    QSpinBox* m_udpPortSelector;
    QPushButton* m_startReceivingButton;
};

} // namespace PacketStream
} // namespace NetworkMiner
