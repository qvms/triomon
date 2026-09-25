#include "LoadingWindow.hpp"
#include <QHBoxLayout>

namespace NetworkMiner {

LoadingWindow::LoadingWindow(const QString& text, QWidget* parent) 
    : QDialog(parent) {

    setWindowTitle("Loading...");
    resize(300, 100);

    QVBoxLayout* layout = new QVBoxLayout(this);
    m_textLabel = new QLabel(text, this);
    layout->addWidget(m_textLabel);

    QHBoxLayout* pLayout = new QHBoxLayout();
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    pLayout->addWidget(m_progressBar);

    m_percentLabel = new QLabel("0 %", this);
    pLayout->addWidget(m_percentLabel);

    layout->addLayout(pLayout);

    m_cancelButton = new QPushButton("Cancel", this);
    layout->addWidget(m_cancelButton);

    connect(m_cancelButton, &QPushButton::clicked, this, &LoadingWindow::onCancel);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &LoadingWindow::updateGui);
    m_timer->start(100);
}

void LoadingWindow::setPercent(int percent) {
    if (percent >= 0 && percent <= 100) {
        m_percent = percent;
    }
}

void LoadingWindow::onCancel() {
    m_isAborted = true;
    close();
}

void LoadingWindow::updateGui() {
    if (m_progressBar->value() != m_percent) {
        m_progressBar->setValue(m_percent);
        m_percentLabel->setText(QString("%1 %").arg(m_percent));
    }
}

void LoadingWindow::closeEvent(QCloseEvent* event) {
    m_isAborted = true;
    m_timer->stop();
    QDialog::closeEvent(event);
}

} // namespace NetworkMiner
