#include "NamedPipe.hpp"
#include <QMessageBox>

namespace NetworkMiner {

NamedPipe::NamedPipe(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Named Pipe Reader");
    resize(300, 120);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_pipeNameTextBox = new QLineEdit("\\\\.\\pipe\\wireshark", this);
    layout->addWidget(new QLabel("Pipe Name:"));
    layout->addWidget(m_pipeNameTextBox);

    m_connectButton = new QPushButton("Connect", this);
    layout->addWidget(m_connectButton);

    connect(m_connectButton, &QPushButton::clicked, this, &NamedPipe::onConnect);
}

void NamedPipe::onConnect() {
    QMessageBox::information(this, "Connected", "Connected to named pipe (mock)");
    accept();
}

} // namespace NetworkMiner
