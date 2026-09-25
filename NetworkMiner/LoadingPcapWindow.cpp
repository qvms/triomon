#include "LoadingPcapWindow.hpp"

namespace NetworkMiner {

LoadingPcapWindow::LoadingPcapWindow(SharedUtils::Pcap::PcapFileReader* pcapReader, const CaseFile& caseFile, QWidget* parent) 
    : LoadingWindow(QString::fromStdString(caseFile.Filename), parent), m_pcapReader(pcapReader), m_caseFile(caseFile) {
    setWindowTitle("Processing Packets");
}

LoadingPcapWindow::~LoadingPcapWindow() {
    if (m_pcapReader) {
        delete m_pcapReader;
    }
}

void LoadingPcapWindow::closeEvent(QCloseEvent* event) {
    if (m_pcapReader) {
        m_pcapReader->AbortFileRead();
    }
    // Handle background worker cancellation here if applicable in Qt (e.g. QThread or QRunnable)
    LoadingWindow::closeEvent(event);
}

} // namespace NetworkMiner
