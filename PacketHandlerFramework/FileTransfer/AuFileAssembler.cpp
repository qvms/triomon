#include "AuFileAssembler.hpp"

AuFileAssembler::AuFileAssembler(const QString& filename, const QHostAddress& sourceHost, quint16 sourcePort, const QHostAddress& destinationHost, quint16 destinationPort, const QDateTime& timestamp)
    : filename(filename), sourceHost(sourceHost), destinationHost(destinationHost), sourcePort(sourcePort), destinationPort(destinationPort), timestamp(timestamp), file(filename)
{
    file.open(QIODevice::WriteOnly);
}

AuFileAssembler::~AuFileAssembler() {
    if (file.isOpen()) {
        finishAssembling();
    }
}

void AuFileAssembler::addData(const QByteArray& data) {
    if (file.isOpen()) {
        file.write(data);
    }
}

void AuFileAssembler::finishAssembling() {
    if (file.isOpen()) {
        file.close();
    }
}
