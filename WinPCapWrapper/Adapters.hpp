#pragma once

#include <QString>
#include <QHostAddress>
#include <vector>

namespace WinPCapWrapper {

class MicroOlapAdapter { // Conceptually implements IPcapAdapter
public:
    MicroOlapAdapter();
    virtual ~MicroOlapAdapter() = default;

    QString Name() const { return "MicroOlap"; }
    QString Description() const { return "MicroOlap Adapter"; }
    bool IsLoopback() const { return false; }
    QString MacAddress() const { return ""; }
    std::vector<QHostAddress> IpAddresses() const { return {}; }
};

class MicroOlapSniffer { // Conceptually implements ISniffer
public:
    MicroOlapSniffer();
    virtual ~MicroOlapSniffer() = default;
};

class NullAdapter { // Conceptually implements IPcapAdapter
public:
    NullAdapter();
    virtual ~NullAdapter() = default;

    QString Name() const { return "Null"; }
    QString Description() const { return "Null Adapter"; }
    bool IsLoopback() const { return false; }
    QString MacAddress() const { return ""; }
    std::vector<QHostAddress> IpAddresses() const { return {}; }
};

} // namespace WinPCapWrapper
