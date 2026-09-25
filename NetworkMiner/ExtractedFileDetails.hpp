#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>

namespace NetworkMiner {

class ExtractedFileDetails : public QDialog {
    Q_OBJECT

public:
    explicit ExtractedFileDetails(const QString& filePath, QWidget* parent = nullptr);
    ~ExtractedFileDetails() override = default;

private slots:
    void updateHexTextBox();

private:
    QString m_filePath;
    uint32_t m_maxBytesToRead = 4096;

    QTableWidget* m_fileDetailsTable;
    QTextEdit* m_contentPreviewTextBox;
    QComboBox* m_showAsComboBox;
    QSpinBox* m_fontSizeNumericUpDown;
    QLineEdit* m_bytesToReadTextBox;
    QLabel* m_identifiedFileTypeLabelText;
    QLabel* m_identifiedFileTypeLabelValue;

    void loadDetails();
};

} // namespace NetworkMiner
