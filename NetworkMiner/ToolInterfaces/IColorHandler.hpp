#pragma once
#include <string>
#include <vector>
namespace NetworkMiner { namespace ToolInterfaces { 
template <typename TKey> class IColorHandler { public: virtual ~IColorHandler() = default; virtual void AddColor(TKey key, const std::string& color) = 0; virtual void RemoveColors(bool updateGuiObjects) = 0; virtual void Clear() = 0; virtual std::vector<TKey> get_Keys() const = 0; virtual bool get_ReloadRequired() const = 0; virtual void RemoveColor(TKey key) = 0; };
} }