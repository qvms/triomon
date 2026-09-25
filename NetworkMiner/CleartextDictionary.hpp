#pragma once

#include <QDialog>

namespace NetworkMiner {

class CleartextDictionary : public QDialog {
    Q_OBJECT

public:
    explicit CleartextDictionary(QWidget* parent = nullptr);
    ~CleartextDictionary() override = default;
};

} // namespace NetworkMiner
