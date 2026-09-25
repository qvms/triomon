#include <QApplication>
#include "MainWindow.hpp"
#include "DatabaseManager.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    DatabaseManager::instance().init();
    
    MainWindow w;
    w.show();
    
    if (argc > 1) {
        w.loadPcap(QString::fromLocal8Bit(argv[1]));
    }
    
    return app.exec();
}
