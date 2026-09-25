#pragma once

#include <QWidget>
#include <QListWidgetItem>

namespace NetworkMiner {

// Deprecated in C#, but implemented for parity
class KeywordFilterControlForListViewItems : public QWidget {
public:
    KeywordFilterControlForListViewItems(QWidget* parent = nullptr) : QWidget(parent) {}
};

} // namespace NetworkMiner
