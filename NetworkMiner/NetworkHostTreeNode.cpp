#include "NetworkHostTreeNode.hpp"

namespace NetworkMiner {

NetworkHostTreeNode::NetworkHostTreeNode(std::shared_ptr<PacketParser::NetworkHost> networkHost, QTreeWidget* parent)
    : QTreeWidgetItem(parent), networkHost(networkHost), iconUpdateRequired(true) {
    UpdateText();
    // Add dummy child for expansion
    QTreeWidgetItem* dummy = new QTreeWidgetItem();
    dummy->setText(0, "dummy node");
    addChild(dummy);
}

void NetworkHostTreeNode::UpdateText() {
    if (!networkHost) return;
    QString text = QString::fromStdString(networkHost->ToString());
    
    // We would fetch Mac Vendor or other info here
    setText(0, text);
    // iconUpdateRequired logic to fetch flags based on MAC etc.
    if (iconUpdateRequired) {
        // e.g. setIcon(0, QIcon("..."));
        iconUpdateRequired = false;
    }
}

void NetworkHostTreeNode::BeforeExpand() {
    // Clear dummy nodes and populate actual data
    takeChildren();
    
    // Stub: we would populate details, OS info, ports, etc. here
    QTreeWidgetItem* detailsNode = new QTreeWidgetItem(this);
    detailsNode->setText(0, "Host Details Placeholder");
}

} // namespace NetworkMiner
