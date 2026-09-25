#pragma once

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QString>

namespace NetworkMiner {

class LoadingWindow : public QDialog {
    Q_OBJECT

public:
    explicit LoadingWindow(const QString& text, QWidget* parent = nullptr);
    ~LoadingWindow() override = default;

    void setPercent(int percent);
    int percent() const { return m_percent; }
    bool isAborted() const { return m_isAborted; }

private slots:
    void onCancel();
    void updateGui();

protected:
    void closeEvent(QCloseEvent* event) override;

    int m_percent = 0;
    bool m_isAborted = false;

    QLabel* m_textLabel;
    QLabel* m_percentLabel;
    QProgressBar* m_progressBar;
    QPushButton* m_cancelButton;
    QTimer* m_timer;
};

} // namespace NetworkMiner
