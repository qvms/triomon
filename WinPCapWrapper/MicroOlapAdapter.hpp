#pragma once

#include <QString>
#include <QList>
#include <memory>

#include "IAdapter.hpp"

// Mock microOLAP SDK to allow compilation of exact ported logic
namespace microOLAP {
namespace PSSDK {

    struct HNAdapterConfig {
        void* Handle;
        QString AdapterDescription;
        QString AdapterName;
        int MaxPacketSize;
    };

    struct PSSDKRES {
        enum Value {
            HNERR_OK = 0
        };
        Value value;
        PSSDKRES(Value v = HNERR_OK) : value(v) {}
        bool operator!=(Value v) const { return value != v; }
        explicit operator int() const { return value; }
    };

    class HNPSManager {
    public:
        void Initialize() {}
        PSSDKRES RefreshAdapterList() { return PSSDKRES(); }
        void* Get_FirstAdapter() { return nullptr; }
        void* Get_NextAdapterCfg(void*) { return nullptr; }
    };

} // namespace PSSDK
} // namespace microOLAP

namespace NetworkWrapper {

class MicroOlapAdapter : public IAdapter {
private:
    microOLAP::PSSDK::HNAdapterConfig adapterConfig;

public:
    static QList<std::shared_ptr<IAdapter>> GetAdapters();

    explicit MicroOlapAdapter(const microOLAP::PSSDK::HNAdapterConfig& adapterConfig);

    microOLAP::PSSDK::HNAdapterConfig getAdapterConfig() const;

    QString ToString() override;
};

} // namespace NetworkWrapper
