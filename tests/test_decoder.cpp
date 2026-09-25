#include <QStringDecoder>
#include <QByteArray>
#include <QString>
#include <iostream>

int main() {
    QStringDecoder decoder("utf-8");
    if (decoder.isValid()) {
        QString result = decoder(QByteArray("test"));
        std::cout << result.toStdString() << std::endl;
    }
    return 0;
}
