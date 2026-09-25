#include "NullAdapter.hpp"

namespace NetworkWrapper {

QString NullAdapter::ToString() const {
    return QStringLiteral("--- Select a network adapter in the list ---");
}

} // namespace NetworkWrapper
