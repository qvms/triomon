#include "NetworkMinerAboutBox.hpp"
#include <QDesktopServices>
#include <QUrl>
#include <QTextEdit>

namespace NetworkMiner {

NetworkMinerAboutBox::NetworkMinerAboutBox(std::map<QString, QString>& localAboutTextNVC, QWidget* parent)
    : QDialog(parent) {
    mainLayout = new QVBoxLayout(this);

    labelProductName = new QLabel("NetworkMiner", this);
    labelVersion = new QLabel("Version: 2.9.x", this);
    labelCopyright = new QLabel("Copyright: NETRESEC", this);
    labelCompanyName = new QLabel("Company: NETRESEC", this);
    
    linkLabelHomepage = new QLabel("<a href=\"https://www.netresec.com/\">https://www.netresec.com/</a>", this);
    linkLabelHomepage->setOpenExternalLinks(true);

    mainLayout->addWidget(labelProductName);
    mainLayout->addWidget(labelVersion);
    mainLayout->addWidget(labelCopyright);
    mainLayout->addWidget(labelCompanyName);
    mainLayout->addWidget(linkLabelHomepage);

    aboutTabControl = new QTabWidget(this);
    mainLayout->addWidget(aboutTabControl);

    if (localAboutTextNVC.find("NetworkMiner") == localAboutTextNVC.end()) {
        localAboutTextNVC["NetworkMiner"] = "NetworkMiner is an open source Network Forensic Analysis Tool...";
    }
    if (localAboutTextNVC.find("Credits") == localAboutTextNVC.end()) {
        localAboutTextNVC["Credits"] = "Uses OS fingerprinting, Satori, Mac-ages, p0f, abuse.ch SSLBL";
    }
    
    for (auto const& [key, val] : localAboutTextNVC) {
        QTextEdit* textEdit = new QTextEdit(this);
        textEdit->setText(val);
        textEdit->setReadOnly(true);
        aboutTabControl->addTab(textEdit, key);
    }
}

void NetworkMinerAboutBox::linkLabelHomepage_Click(const QString& link) {
    QDesktopServices::openUrl(QUrl(link));
}

} // namespace NetworkMiner
