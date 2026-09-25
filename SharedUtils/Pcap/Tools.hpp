#pragma once
#include <QString>
#include <QDateTime>

namespace SharedUtils {
namespace Pcap {

class Tools {
public:
    static QString GenerateCaptureFileName(const QDateTime& timestamp) {
        QString timeStr = timestamp.toString(Qt::ISODate);
        timeStr.replace(':', '-');
        return "NM_" + timeStr + ".pcap";
    }
};

}
}
