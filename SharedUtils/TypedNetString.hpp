#pragma once
#include <vector>
#include <string>

namespace SharedUtils {

class TypedNetString {
public:
    enum class TypeSuffix : uint8_t {
        ASCIIString = ',',
        Boolean = '!',
        Dictionary = '}',
        Float = '^',
        Integer = '#',
        List = ']',
        UTF8String = ';',
        Null = '~'
    };

    // Need a stream abstraction if we actually read from it, for now we will just mock the class structure
    TypeSuffix Type;
    
    // In a real C++ implementation we'd probably use QIODevice or std::istream
    // This is just to satisfy the class presence.
    // TypedNetString(QIODevice* data);
    
    std::vector<uint8_t> GetRawValue() const;
    // GetValue() would ideally return std::variant or similar.
};

}
