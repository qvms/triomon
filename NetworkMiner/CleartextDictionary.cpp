#include "CleartextDictionary.hpp"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace NetworkMiner {

CleartextDictionary::CleartextDictionary(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Cleartext Dictionary");
    resize(300, 200);

    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Dictionary management will be implemented here.", this);
    QPushButton* okButton = new QPushButton("OK", this);

    layout->addWidget(label);
    layout->addWidget(okButton);
    setLayout(layout);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
}

} // namespace NetworkMiner
