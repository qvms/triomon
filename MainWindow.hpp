#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <vector>
#include <memory>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "PacketParser/Frame.hpp"
#include "NetworkMiner/PacketHandlerWrapper.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
    void loadPcap(const QString& filename);
    
protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    
private slots:
    void onOpenPcap();
    void onReceivePcapOverTcp();
    void onReceiveTzsp();
    void onOpenNamedPipe();
    void onClear();
    
private:
    void setupUi();
    
    QTabWidget* m_tabWidget;
    
    // Tabs
    QTreeWidget* m_hostsTree;
    QTableWidget* m_filesTable;
    QListWidget* m_imagesList;
    QTableWidget* m_messagesTable;
    QTableWidget* m_credentialsTable;
    QTableWidget* m_sessionsTable;
    QTableWidget* m_dnsTable;
    QTableWidget* m_parametersTable;
    QTableWidget* m_keywordsTable;
    QTableWidget* m_cleartextTable;
    QTableWidget* m_anomaliesTable;
    
    std::vector<std::shared_ptr<PacketParser::Frame>> m_frames;
    std::shared_ptr<NetworkMiner::PacketHandlerWrapper> m_handlerWrapper;
    
    void UpdateUI();
};
