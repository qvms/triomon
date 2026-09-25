#include "TypedNetString.hpp"
#include <stdexcept>

namespace SharedUtils {

std::vector<uint8_t> TypedNetString::GetRawValue() const {
    throw std::runtime_error("Not implemented in mock");
}

}
