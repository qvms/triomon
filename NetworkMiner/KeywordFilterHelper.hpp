#pragma once

#include <QWidget>
#include <QString>
#include <QColor>
#include <QLineEdit>

namespace NetworkMiner {

class KeywordFilterHelper : public QObject {
    Q_OBJECT
private:
    static const QString DEFAULT_HELPER_TEXT;
    QColor helperTextColor;
    QColor userTextColor;
    QString helperText;
    QLineEdit* keywordBox;

public:
    explicit KeywordFilterHelper(QLineEdit* keywordBox, const QString& helperText = DEFAULT_HELPER_TEXT, QObject* parent = nullptr);

    QColor HelperTextColor() const { return helperTextColor; }
    void setHelperTextColor(const QColor& color) { helperTextColor = color; }

    QColor UserTextColor() const { return userTextColor; }
    void setUserTextColor(const QColor& color) { userTextColor = color; }

    QString GetFilterText() const;
    void Reset(bool keywordBoxIsActive);
    void Reset(QWidget* activeControl);
    bool CurrentTextIsHelperText() const;

private slots:
    void KeywordBox_Enter();
};

} // namespace NetworkMiner
