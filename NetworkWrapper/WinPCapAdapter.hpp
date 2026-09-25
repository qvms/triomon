#pragma once

#include "IAdapter.hpp"
#include "WinPCapWrapper.hpp"
#include <QString>
#include <QList>
#include <memory>

namespace NetworkWrapper {

class WinPCapAdapter : public IAdapter {
private:
    QString npfName;
    QString description;
    QString ipAddress;
    QString netmask;

public:
    WinPCapAdapter(const Device& device);

    QString NPFName() const { return npfName; }

    QString ToString() const override;

    static QList<std::shared_ptr<IAdapter>> GetAdapters();
};

} // namespace NetworkWrapper
