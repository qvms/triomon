#pragma once

#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <map>
#include <QString>
#include <QVBoxLayout>

namespace NetworkMiner {

class NetworkMinerAboutBox : public QDialog {
    Q_OBJECT
private:
    QLabel* labelProductName;
    QLabel* labelVersion;
    QLabel* labelCopyright;
    QLabel* labelCompanyName;
    QLabel* linkLabelHomepage;
    QTabWidget* aboutTabControl;
    QVBoxLayout* mainLayout;

public:
    NetworkMinerAboutBox(std::map<QString, QString>& localAboutTextNVC, QWidget* parent = nullptr);
    virtual ~NetworkMinerAboutBox() = default;

private slots:
    void linkLabelHomepage_Click(const QString& link);
};

} // namespace NetworkMiner
