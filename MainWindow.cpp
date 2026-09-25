#include "PacketParser/PacketHandler.hpp"
#include "DatabaseManager.hpp"
#include "MainWindow.hpp"
#include <QHeaderView>
#include <QDebug>
#include <QProgressDialog>
#include <QToolBar>
#include <QStatusBar>
#include <pcap.h>
#include "PacketParser/Packets/IPv4Packet.hpp"
#include "PacketParser/Packets/IPv6Packet.hpp"
#include "PacketParser/Packets/TcpPacket.hpp"
#include "PacketParser/Packets/UdpPacket.hpp"
#include <QHostAddress>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Qt 6 PCAP Application");
    resize(1024, 768);

    setupUi();
    setAcceptDrops(true);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    // Menus
    QMenu* fileMenu = menuBar()->addMenu("&File");
    QAction* openAction = new QAction("&Open PCAP...", this);
    fileMenu->addAction(openAction);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenPcap);

    QAction* receiveTcpAction = new QAction("Receive PCAP over TCP...", this);
    fileMenu->addAction(receiveTcpAction);
    connect(receiveTcpAction, &QAction::triggered, this, &MainWindow::onReceivePcapOverTcp);

    QAction* receiveTzspAction = new QAction("Receive TZSP...", this);
    fileMenu->addAction(receiveTzspAction);
    connect(receiveTzspAction, &QAction::triggered, this, &MainWindow::onReceiveTzsp);

    QAction* namedPipeAction = new QAction("Read from Named Pipe...", this);
    fileMenu->addAction(namedPipeAction);
    connect(namedPipeAction, &QAction::triggered, this, &MainWindow::onOpenNamedPipe);

    fileMenu->addSeparator();
    QAction* clearAction = new QAction("&Clear All", this);
    fileMenu->addAction(clearAction);
    connect(clearAction, &QAction::triggered, this, &MainWindow::onClear);
    
    fileMenu->addSeparator();
    QAction* exitAction = new QAction("E&xit", this);
    fileMenu->addAction(exitAction);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // Toolbar
    QToolBar* toolbar = addToolBar("Main");
    toolbar->addAction(openAction);
    toolbar->addAction(clearAction);

    // Tab Widget
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    // Hosts
    m_hostsTree = new QTreeWidget();
    m_hostsTree->setHeaderLabels({"Host", "IP Address", "MAC Address", "OS", "Open Ports"});
    m_tabWidget->addTab(m_hostsTree, "Hosts");

    // Files
    m_filesTable = new QTableWidget(0, 6);
    m_filesTable->setHorizontalHeaderLabels({"Filename", "Size", "Source", "Destination", "Protocol", "Timestamp"});
    m_filesTable->horizontalHeader()->setStretchLastSection(true);
    m_filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabWidget->addTab(m_filesTable, "Files");

    // Images
    m_imagesList = new QListWidget();
    m_imagesList->setViewMode(QListView::IconMode);
    m_imagesList->setIconSize(QSize(128, 128));
    m_imagesList->setResizeMode(QListView::Adjust);
    m_tabWidget->addTab(m_imagesList, "Images");

    // Messages
    m_messagesTable = new QTableWidget(0, 5);
    m_messagesTable->setHorizontalHeaderLabels({"From", "To", "Protocol", "Subject", "Timestamp"});
    m_messagesTable->horizontalHeader()->setStretchLastSection(true);
    m_messagesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabWidget->addTab(m_messagesTable, "Messages");

    // Credentials
    m_credentialsTable = new QTableWidget(0, 5);
    m_credentialsTable->setHorizontalHeaderLabels({"Client", "Server", "Protocol", "Username", "Password"});
    m_credentialsTable->horizontalHeader()->setStretchLastSection(true);
    m_credentialsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabWidget->addTab(m_credentialsTable, "Credentials");

    // Sessions
    m_sessionsTable = new QTableWidget(0, 7);
    m_sessionsTable->setHorizontalHeaderLabels({"Client IP", "Client Port", "Server IP", "Server Port", "Protocol", "Frames", "Timestamp"});
    m_sessionsTable->horizontalHeader()->setStretchLastSection(true);
    m_sessionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabWidget->addTab(m_sessionsTable, "Sessions");

    // DNS
    m_dnsTable = new QTableWidget(0, 5);
    m_dnsTable->setHorizontalHeaderLabels({"Client", "Server", "Query", "Response", "Timestamp"});
    m_dnsTable->horizontalHeader()->setStretchLastSection(true);
    m_dnsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabWidget->addTab(m_dnsTable, "DNS");

    // Parameters
    m_parametersTable = new QTableWidget(0, 5);
    m_parametersTable->setHorizontalHeaderLabels({"Name", "Value", "Source", "Destination", "Timestamp"});
    m_parametersTable->horizontalHeader()->setStretchLastSection(true);
    m_parametersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabWidget->addTab(m_parametersTable, "Parameters");

    // Keywords
    m_keywordsTable = new QTableWidget(0, 4);
    m_keywordsTable->setHorizontalHeaderLabels({"Keyword", "Context", "Source", "Destination"});
    m_keywordsTable->horizontalHeader()->setStretchLastSection(true);
    m_keywordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabWidget->addTab(m_keywordsTable, "Keywords");

    // Cleartext
    m_cleartextTable = new QTableWidget(0, 4);
    m_cleartextTable->setHorizontalHeaderLabels({"Text", "Protocol", "Source", "Destination"});
    m_cleartextTable->horizontalHeader()->setStretchLastSection(true);
    m_cleartextTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabWidget->addTab(m_cleartextTable, "Cleartext");

    // Anomalies
    m_anomaliesTable = new QTableWidget(0, 4);
    m_anomaliesTable->setHorizontalHeaderLabels({"Anomaly", "Severity", "Source", "Destination"});
    m_anomaliesTable->horizontalHeader()->setStretchLastSection(true);
    m_anomaliesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabWidget->addTab(m_anomaliesTable, "Anomalies");

    statusBar()->showMessage("Ready");
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.isEmpty()) {
            QString filePath = urlList.first().toLocalFile();
            loadPcap(filePath);
        }
    }
}

void MainWindow::onOpenPcap() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open PCAP File", "", "PCAP Files (*.pcap);;All Files (*)");
    if (!fileName.isEmpty()) {
        loadPcap(fileName);
    }
}

void MainWindow::onReceivePcapOverTcp() {
    QMessageBox::information(this, "Not Implemented", "Receive PCAP over TCP form will be shown here.");
}

void MainWindow::onReceiveTzsp() {
    QMessageBox::information(this, "Not Implemented", "Receive TZSP form will be shown here.");
}

void MainWindow::onOpenNamedPipe() {
    QMessageBox::information(this, "Not Implemented", "Named Pipe form will be shown here.");
}

void MainWindow::onClear() {
    m_frames.clear();
    m_hostsTree->clear();
    m_filesTable->setRowCount(0);
    m_imagesList->clear();
    m_messagesTable->setRowCount(0);
    m_credentialsTable->setRowCount(0);
    m_sessionsTable->setRowCount(0);
    m_dnsTable->setRowCount(0);
    m_parametersTable->setRowCount(0);
    m_keywordsTable->setRowCount(0);
    m_cleartextTable->setRowCount(0);
    m_anomaliesTable->setRowCount(0);
    statusBar()->showMessage("Cleared");
}

void MainWindow::loadPcap(const QString& filename) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = pcap_open_offline(filename.toLocal8Bit().constData(), errbuf);
    if (!pcap) {
        QMessageBox::critical(this, "Error", QString("Failed to open pcap: %1").arg(errbuf));
        return;
    }

    statusBar()->showMessage(QString("Loading %1...").arg(filename));

    int dlt = pcap_datalink(pcap);
    PacketParser::DataLinkTypeEnum linkType;
    switch (dlt) {
        case DLT_EN10MB:
            linkType = PacketParser::DataLinkTypeEnum::WTAP_ENCAP_ETHERNET;
            break;
        case DLT_IEEE802_11:
            linkType = PacketParser::DataLinkTypeEnum::WTAP_ENCAP_IEEE_802_11;
            break;
        case DLT_IEEE802_11_RADIO:
            linkType = PacketParser::DataLinkTypeEnum::WTAP_ENCAP_IEEE_802_11_WLAN_RADIOTAP;
            break;
        case DLT_RAW:
            linkType = PacketParser::DataLinkTypeEnum::WTAP_ENCAP_RAW_IP;
            break;
        case DLT_LINUX_SLL:
            linkType = PacketParser::DataLinkTypeEnum::WTAP_ENCAP_SLL;
            break;
        case DLT_NULL:
            linkType = PacketParser::DataLinkTypeEnum::WTAP_ENCAP_NULL;
            break;
        default:
            linkType = static_cast<PacketParser::DataLinkTypeEnum>(dlt);
            qDebug() << "Unknown DLT:" << dlt;
            break;
    }

    struct pcap_pkthdr* header;
    const u_char* data;
    int res;
    int64_t frameNumber = 1;

    m_sessionsTable->setUpdatesEnabled(false);
    
    PacketParser::PacketHandler handler;

    while ((res = pcap_next_ex(pcap, &header, &data)) >= 0) {
        if (res == 0) continue;

        QDateTime ts;
        ts.setSecsSinceEpoch(header->ts.tv_sec);
        
        auto frame = std::make_shared<PacketParser::Frame>(
            ts, 
            data, 
            header->caplen, 
            frameNumber++, 
            linkType, 
            true,
            false
        );

        handler.ParseFrame(frame);
        m_frames.push_back(frame);
    }

    pcap_close(pcap);
    
    // Populate UI and DB from handler
    const auto& sessions = handler.GetSessions();
    for (const auto& sess : sessions) {
        int row = m_sessionsTable->rowCount();
        m_sessionsTable->insertRow(row);
        m_sessionsTable->setItem(row, 0, new QTableWidgetItem(sess.clientIp));
        m_sessionsTable->setItem(row, 1, new QTableWidgetItem(QString::number(sess.clientPort)));
        m_sessionsTable->setItem(row, 2, new QTableWidgetItem(sess.serverIp));
        m_sessionsTable->setItem(row, 3, new QTableWidgetItem(QString::number(sess.serverPort)));
        m_sessionsTable->setItem(row, 4, new QTableWidgetItem(sess.protocol));
        m_sessionsTable->setItem(row, 5, new QTableWidgetItem(QString("Len: %1").arg(sess.frameLength)));
        m_sessionsTable->setItem(row, 6, new QTableWidgetItem(sess.timestamp.toString("HH:mm:ss.zzz")));
        
        DatabaseManager::instance().insertSession(
            sess.clientIp, QString::number(sess.clientPort),
            sess.serverIp, QString::number(sess.serverPort),
            sess.protocol, QString("Len: %1").arg(sess.frameLength),
            sess.timestamp.toString("HH:mm:ss.zzz")
        );
    }

    auto hosts = handler.GetNetworkHostList()->Hosts();
    m_hostsTree->clear();
    for (const auto& host : hosts) {
        QTreeWidgetItem* hostItem = new QTreeWidgetItem(m_hostsTree);
        hostItem->setText(0, QString::fromStdString(host->ToString()));
    }

    m_sessionsTable->setUpdatesEnabled(true);

    statusBar()->showMessage(QString("Loaded %1 frames").arg(m_frames.size()));
}
