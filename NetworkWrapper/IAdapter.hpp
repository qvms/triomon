#pragma once
#include <QString>
#include <QHostAddress>
#include <QByteArray>
#include <QDateTime>
#include <QList>
#include <QHash>

namespace NetworkWrapper {
class IAdapter {
public:
    virtual ~IAdapter() = default;

    virtual QString ToString() const = 0;
};

} // namespace NetworkWrapper
