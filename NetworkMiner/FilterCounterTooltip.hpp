#pragma once

#include <QString>
#include <QToolTip>

namespace NetworkMiner {

class FilterCounterTooltip {
public:
    static void Show(int matchedItems, int totalItems, int delayMs = 1500) {
        QString text = QString("Matched %1 / %2").arg(matchedItems).arg(totalItems);
        // Show tooltip globally or near mouse cursor; for now just centered or using QToolTip
        QToolTip::showText(QCursor::pos(), text);
    }
};

} // namespace NetworkMiner
