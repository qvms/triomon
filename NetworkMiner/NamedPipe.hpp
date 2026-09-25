#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

namespace NetworkMiner {

class NamedPipe : public QDialog {
    Q_OBJECT

public:
    explicit NamedPipe(QWidget* parent = nullptr);
    ~NamedPipe() override = default;

private slots:
    void onConnect();

private:
    QLineEdit* m_pipeNameTextBox;
    QPushButton* m_connectButton;
};

} // namespace NetworkMiner
