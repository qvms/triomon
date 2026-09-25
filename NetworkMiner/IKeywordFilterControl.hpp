#pragma once
#include <vector>
namespace NetworkMiner { template <typename T> class IKeywordFilterControl { public: virtual ~IKeywordFilterControl() = default; virtual void Add(const T& item) = 0; virtual int AddRange(const std::vector<T>& items) = 0; virtual void Clear() = 0; }; }