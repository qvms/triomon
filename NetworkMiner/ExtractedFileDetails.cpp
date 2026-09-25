#include "ExtractedFileDetails.hpp"
#include <QFileInfo>
#include <QFile>
#include <QHeaderView>

namespace NetworkMiner {

ExtractedFileDetails::ExtractedFileDetails(const QString& filePath, QWidget* parent) 
    : QDialog(parent), m_filePath(filePath) {

    setWindowTitle("File Details");
    resize(600, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Properties table
    m_fileDetailsTable = new QTableWidget(0, 2, this);
    m_fileDetailsTable->setHorizontalHeaderLabels({"Property", "Value"});
    m_fileDetailsTable->horizontalHeader()->setStretchLastSection(true);
    m_fileDetailsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fileDetailsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_fileDetailsTable);

    loadDetails();

    // Controls
    QHBoxLayout* controlsLayout = new QHBoxLayout();
    m_showAsComboBox = new QComboBox(this);
    m_showAsComboBox->addItems({"Hexdump", "ASCII", "UTF-8", "Unicode"});
    controlsLayout->addWidget(new QLabel("Show As:"));
    controlsLayout->addWidget(m_showAsComboBox);

    m_bytesToReadTextBox = new QLineEdit(QString::number(m_maxBytesToRead), this);
    controlsLayout->addWidget(new QLabel("Max Bytes:"));
    controlsLayout->addWidget(m_bytesToReadTextBox);

    m_identifiedFileTypeLabelText = new QLabel("Identified Type:", this);
    m_identifiedFileTypeLabelValue = new QLabel("", this);
    controlsLayout->addWidget(m_identifiedFileTypeLabelText);
    controlsLayout->addWidget(m_identifiedFileTypeLabelValue);
    controlsLayout->addStretch();

    mainLayout->addLayout(controlsLayout);

    // Content preview
    m_contentPreviewTextBox = new QTextEdit(this);
    m_contentPreviewTextBox->setReadOnly(true);
    QFont font("Courier");
    m_contentPreviewTextBox->setFont(font);
    mainLayout->addWidget(m_contentPreviewTextBox);

    connect(m_showAsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ExtractedFileDetails::updateHexTextBox);
    connect(m_bytesToReadTextBox, &QLineEdit::returnPressed, this, &ExtractedFileDetails::updateHexTextBox);

    updateHexTextBox();
}

void ExtractedFileDetails::loadDetails() {
    QFileInfo fi(m_filePath);
    m_fileDetailsTable->setRowCount(0);
    
    auto addRow = [&](const QString& prop, const QString& val) {
        int row = m_fileDetailsTable->rowCount();
        m_fileDetailsTable->insertRow(row);
        m_fileDetailsTable->setItem(row, 0, new QTableWidgetItem(prop));
        m_fileDetailsTable->setItem(row, 1, new QTableWidgetItem(val));
    };

    addRow("Name", fi.fileName());
    addRow("Size", QString::number(fi.size()));
    addRow("Path", fi.absoluteFilePath());
    addRow("LastModified", fi.lastModified().toString(Qt::ISODate));
}

void ExtractedFileDetails::updateHexTextBox() {
    bool ok;
    uint32_t bytes = m_bytesToReadTextBox->text().toUInt(&ok);
    if (ok) {
        m_maxBytesToRead = bytes;
    } else {
        m_bytesToReadTextBox->setText(QString::number(m_maxBytesToRead));
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_contentPreviewTextBox->setText("Failed to open file.");
        return;
    }

    QByteArray data = file.read(m_maxBytesToRead);
    int idx = m_showAsComboBox->currentIndex();

    if (idx == 0) { // Hexdump
        QString hex;
        for (int i = 0; i < data.size(); i += 16) {
            QString line = QString("%1: ").arg(i, 8, 16, QChar('0'));
            QString textPart;
            for (int j = 0; j < 16; j++) {
                if (i + j < data.size()) {
                    unsigned char c = data[i+j];
                    line += QString("%1 ").arg(c, 2, 16, QChar('0'));
                    textPart += (c >= 32 && c <= 126) ? QChar(c) : QChar('.');
                } else {
                    line += "   ";
                }
            }
            line += "  " + textPart + "\n";
            hex += line;
        }
        m_contentPreviewTextBox->setText(hex);
    } else if (idx == 1) { // ASCII
        m_contentPreviewTextBox->setText(QString::fromLatin1(data));
    } else if (idx == 2) { // UTF-8
        m_contentPreviewTextBox->setText(QString::fromUtf8(data));
    } else { // Unicode (UTF-16)
        m_contentPreviewTextBox->setText(QString::fromUtf16(reinterpret_cast<const char16_t*>(data.constData()), data.size() / 2));
    }
}

} // namespace NetworkMiner
