#pragma once

#include <QWidget>
#include <QTreeWidgetItem>

// Forward declaration of the generic KeywordFilterControl
template <typename T>
class KeywordFilterControl;

namespace NetworkMiner {

class KeywordFilterControlForTreeNodes : public QWidget { // Assuming KeywordFilterControl maps to QWidget
    // In Qt we might not need this exact generic inheritance trick for designer,
    // but preserving class parity.
public:
    void Add(QTreeWidgetItem* item);
    // You would map UnfilteredList and AddItemCallback here or in a base class
};

} // namespace NetworkMiner
