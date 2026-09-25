#pragma once
#include <vector>
#include <functional>
#include <memory>

namespace SharedUtils {

// C++ equivalent to IEnumerator
template <typename T>
class IEnumerator {
public:
    virtual ~IEnumerator() = default;
    virtual bool MoveNext() = 0;
    virtual T Current() const = 0;
    virtual void Reset() = 0;
};

// C++ equivalent to IEnumerable
template <typename T>
class IEnumerable {
public:
    virtual ~IEnumerable() = default;
    virtual std::unique_ptr<IEnumerator<T>> GetEnumerator() const = 0;
};

template <typename T>
class MergedEnumerable : public IEnumerable<T> {
private:
    std::function<bool(const T&, const T&)> isBetter;
    std::vector<std::unique_ptr<IEnumerable<T>>> enumerables;

public:
    // Move vector into class to own the enumerables
    MergedEnumerable(std::vector<std::unique_ptr<IEnumerable<T>>>&& enumerables, std::function<bool(const T&, const T&)> isBetter)
        : isBetter(isBetter), enumerables(std::move(enumerables)) {}

    class AggregatedEnumerator : public IEnumerator<T> {
    private:
        std::vector<std::unique_ptr<IEnumerator<T>>> enumerators;
        IEnumerator<T>* currentEnumerator;
        std::function<bool(const T&, const T&)> isBetter;

    public:
        AggregatedEnumerator(std::vector<std::unique_ptr<IEnumerator<T>>>&& enums, std::function<bool(const T&, const T&)> better)
            : enumerators(std::move(enums)), isBetter(better), currentEnumerator(nullptr) {}

        bool MoveNext() override {
            if (currentEnumerator == nullptr) {
                auto it = enumerators.begin();
                while (it != enumerators.end()) {
                    if (!(*it)->MoveNext()) {
                        it = enumerators.erase(it);
                    } else {
                        ++it;
                    }
                }
            } else {
                if (!currentEnumerator->MoveNext()) {
                    auto it = std::find_if(enumerators.begin(), enumerators.end(),
                                           [this](const std::unique_ptr<IEnumerator<T>>& e) {
                                               return e.get() == currentEnumerator;
                                           });
                    if (it != enumerators.end()) {
                        enumerators.erase(it);
                    }
                }
            }

            currentEnumerator = nullptr;
            for (auto& enumerator : enumerators) {
                if (currentEnumerator == nullptr || isBetter(enumerator->Current(), currentEnumerator->Current())) {
                    currentEnumerator = enumerator.get();
                }
            }
            return currentEnumerator != nullptr;
        }

        T Current() const override {
            if (currentEnumerator) {
                return currentEnumerator->Current();
            }
            return T();
        }

        void Reset() override {
            currentEnumerator = nullptr;
            for (auto& enumerator : enumerators) {
                enumerator->Reset();
            }
        }
    };

    std::unique_ptr<IEnumerator<T>> GetEnumerator() const override {
        std::vector<std::unique_ptr<IEnumerator<T>>> enumerators;
        for (const auto& eles : enumerables) {
            enumerators.push_back(eles->GetEnumerator());
        }
        return std::make_unique<AggregatedEnumerator>(std::move(enumerators), isBetter);
    }
};

}
