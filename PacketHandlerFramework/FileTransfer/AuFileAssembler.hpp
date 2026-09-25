#ifndef AUFILEASSEMBLER_HPP
#define AUFILEASSEMBLER_HPP

#include <QString>
#include <QHostAddress>
#include <QDateTime>
#include <QFile>

class AuFileAssembler {
public:
    AuFileAssembler(const QString& filename, const QHostAddress& sourceHost, quint16 sourcePort, const QHostAddress& destinationHost, quint16 destinationPort, const QDateTime& timestamp);
    ~AuFileAssembler();

    void addData(const QByteArray& data);
    void finishAssembling();

private:
    QString filename;
    QHostAddress sourceHost;
    QHostAddress destinationHost;
    quint16 sourcePort;
    quint16 destinationPort;
    QDateTime timestamp;
    QFile file;
};

#endif // AUFILEASSEMBLER_HPP
