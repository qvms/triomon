#pragma once

#include <QWidget>
#include <QTreeWidgetItem>
#include <vector>

namespace NetworkMiner {

class GuiExtensions {
public:
    static void SetDoubleBuffered(QWidget* control, bool enable) {
        // In Qt, DoubleBuffering is enabled by default via Qt::WA_PaintOnScreen etc.
        // We can just set an attribute if we want, but it's generally ignored in Qt5/6 as it's the default.
        if (control) {
            control->setAttribute(Qt::WA_NoSystemBackground, !enable);
        }
    }

    static std::vector<QTreeWidgetItem*> GetOpenChildTreeNodes(QTreeWidgetItem* treeNode) {
        std::vector<QTreeWidgetItem*> result;
        if (treeNode && treeNode->isExpanded()) {
            for (int i = 0; i < treeNode->childCount(); ++i) {
                QTreeWidgetItem* child = treeNode->child(i);
                result.push_back(child);
                if (child->isExpanded()) {
                    auto grandChildren = GetOpenChildTreeNodes(child);
                    result.insert(result.end(), grandChildren.begin(), grandChildren.end());
                }
            }
        }
        return result;
    }
};

} // namespace NetworkMiner
