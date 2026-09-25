#include "WinPCapAdapter.hpp"

namespace NetworkWrapper {

WinPCapAdapter::WinPCapAdapter(const Device& device) {
    this->ipAddress = device.Address;
    this->description = device.Description;
    this->npfName = device.Name;
    this->netmask = device.Netmask;
}

QString WinPCapAdapter::ToString() const {
    QString returnString = "WinPcap: " + this->description;
    if (!ipAddress.isEmpty() && ipAddress.length() > 6) {
        returnString += " (" + ipAddress + ")";
    }
    if (npfName.contains("{")) {
        returnString += " " + npfName.mid(npfName.indexOf('{'));
    } else {
        returnString += " " + npfName;
    }
    return returnString;
}

QList<std::shared_ptr<IAdapter>> WinPCapAdapter::GetAdapters() {
    QList<std::shared_ptr<IAdapter>> deviceList;
    for (const Device& d : WinPCapWrapper::FindAllDevs()) {
        deviceList.append(std::make_shared<WinPCapAdapter>(d));
    }
    return deviceList;
}

} // namespace NetworkWrapper
