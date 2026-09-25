#include "KeywordFilterHelper.hpp"

namespace NetworkMiner {

const QString KeywordFilterHelper::DEFAULT_HELPER_TEXT = "Enter a keyword filter";

KeywordFilterHelper::KeywordFilterHelper(QLineEdit* keywordBox, const QString& helperText, QObject* parent)
    : QObject(parent), keywordBox(keywordBox), helperText(helperText), 
      helperTextColor(Qt::gray), userTextColor(Qt::black) {
      
    this->keywordBox->setText(this->helperText);
    
    // Qt way to set placeholder text color is usually via stylesheet, or by connecting to focus events
    // We'll mimic the focus event behavior from C#
    connect(keywordBox, &QLineEdit::cursorPositionChanged, this, [this](int oldPos, int newPos) {
        if (oldPos == 0 && newPos > 0 && CurrentTextIsHelperText()) {
            KeywordBox_Enter();
        }
    });
}

QString KeywordFilterHelper::GetFilterText() const {
    if (keywordBox->text() == helperText) {
        return QString();
    }
    return keywordBox->text();
}

void KeywordFilterHelper::Reset(bool keywordBoxIsActive) {
    if (keywordBoxIsActive) {
        keywordBox->setText(QString());
    } else {
        keywordBox->setText(helperText);
    }
}

void KeywordFilterHelper::Reset(QWidget* activeControl) {
    Reset(keywordBox == activeControl);
}

bool KeywordFilterHelper::CurrentTextIsHelperText() const {
    return keywordBox->text() == helperText;
}

void KeywordFilterHelper::KeywordBox_Enter() {
    if (CurrentTextIsHelperText()) {
        keywordBox->setText(QString());
    }
}

} // namespace NetworkMiner
