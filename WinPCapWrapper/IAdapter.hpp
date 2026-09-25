#pragma once

#include <QString>

namespace WinPCapWrapper {

class IAdapter {
public:
    virtual ~IAdapter() = default;

    virtual QString toString() const = 0;
};

} // namespace WinPCapWrapper