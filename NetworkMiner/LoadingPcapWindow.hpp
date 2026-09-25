#pragma once

#include "LoadingWindow.hpp"
#include "CaseFile.hpp" // For CaseFile mock

// Forward declaration for PcapFileReader equivalent
namespace SharedUtils {
    namespace Pcap {
        class PcapFileReader {
        public:
            virtual ~PcapFileReader() = default;
            virtual void AbortFileRead() = 0;
        };
    }
}

namespace NetworkMiner {

class LoadingPcapWindow : public LoadingWindow {
    Q_OBJECT

public:
    LoadingPcapWindow(SharedUtils::Pcap::PcapFileReader* pcapReader, const CaseFile& caseFile, QWidget* parent = nullptr);
    ~LoadingPcapWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    SharedUtils::Pcap::PcapFileReader* m_pcapReader;
    CaseFile m_caseFile;
};

} // namespace NetworkMiner
