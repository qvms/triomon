#pragma once
#include <string>
// Since C++ doesn't have native NewtonSoft.Json equivalent built-in without qt or external libs,
// we just provide a mocked structure of DateTimeAsUtcConverter that can be integrated later.

namespace SharedUtils {

class DateTimeAsUtcConverter {
public:
    virtual ~DateTimeAsUtcConverter() = default;
    
    // In actual implementation we'd override ReadJson and WriteJson
    // from a JSON library like nlohmann::json or QJsonValue
    
    bool CanConvert(const std::string& objectType) const {
        return objectType == "DateTime" || objectType == "System.DateTime";
    }
};

}