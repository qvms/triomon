#pragma once

#include "IAdapter.hpp"
#include <QString>

namespace NetworkWrapper {

class NullAdapter : public IAdapter {
public:
    NullAdapter() = default;
    ~NullAdapter() override = default;

    QString ToString() const override;
};

} // namespace NetworkWrapper
