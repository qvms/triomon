#ifndef ITCPSREAMASSEMBLER_HPP
#define ITCPSREAMASSEMBLER_HPP

#include <QByteArray>

class ITcpStreamAssembler {
public:
    virtual ~ITcpStreamAssembler() = default;
    virtual void addData(const QByteArray& data, quint32 sequenceNumber) = 0;
    virtual void finishAssembling() = 0;
};

#endif // ITCPSREAMASSEMBLER_HPP
