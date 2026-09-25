#include "MicroOlapAdapter.hpp"
#include <QList>
#include <QString>
#include <memory>

namespace NetworkWrapper {

QList<std::shared_ptr<IAdapter>> MicroOlapAdapter::GetAdapters() {
    QList<std::shared_ptr<IAdapter>> deviceList;

    microOLAP::PSSDK::HNPSManager hnpsManager;
    hnpsManager.Initialize();

    microOLAP::PSSDK::PSSDKRES pssdkres = hnpsManager.RefreshAdapterList();
    void* acHandle = hnpsManager.Get_FirstAdapter();

    while (acHandle != nullptr) {
        microOLAP::PSSDK::HNAdapterConfig adapterConfig;
        adapterConfig.Handle = acHandle;
        deviceList.append(std::make_shared<MicroOlapAdapter>(adapterConfig));
        acHandle = hnpsManager.Get_NextAdapterCfg(acHandle);
    }

    return deviceList;
}

MicroOlapAdapter::MicroOlapAdapter(const microOLAP::PSSDK::HNAdapterConfig& adapterConfig)
    : adapterConfig(adapterConfig) {
}

microOLAP::PSSDK::HNAdapterConfig MicroOlapAdapter::getAdapterConfig() const {
    return this->adapterConfig;
}

QString MicroOlapAdapter::ToString() {
    QString returnString = "MicroOLAP: " + this->adapterConfig.AdapterDescription;
    if (this->adapterConfig.AdapterName.contains("{")) {
        returnString.append(" " + this->adapterConfig.AdapterName.mid(this->adapterConfig.AdapterName.indexOf('{')));
    } else {
        returnString.append(" " + this->adapterConfig.AdapterName);
    }
    return returnString;
}

} // namespace NetworkWrapper
