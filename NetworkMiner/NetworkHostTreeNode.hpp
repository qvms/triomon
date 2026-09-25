#pragma once

#include <QTreeWidgetItem>
#include <QIcon>
#include "PacketParser/NetworkHost.hpp"

namespace NetworkMiner {

class IBeforeExpand {
public:
    virtual void BeforeExpand() = 0;
    virtual ~IBeforeExpand() = default;
};

class NetworkHostTreeNode : public QTreeWidgetItem, public IBeforeExpand {
private:
    std::shared_ptr<PacketParser::NetworkHost> networkHost;
    bool iconUpdateRequired;
    
public:
    NetworkHostTreeNode(std::shared_ptr<PacketParser::NetworkHost> networkHost, QTreeWidget* parent = nullptr);

    std::shared_ptr<PacketParser::NetworkHost> GetNetworkHost() const { return networkHost; }
    void FlagIconUpdateRequired() { iconUpdateRequired = true; }

    void UpdateText();
    void BeforeExpand() override;
};

} // namespace NetworkMiner
