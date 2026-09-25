#ifndef QUEUETHRESHOLDSIGNALLER_HPP
#define QUEUETHRESHOLDSIGNALLER_HPP

#include <QQueue>
#include <QWaitCondition>
#include <QMutex>

template <typename T>
class QueueThresholdSignaller {
public:
    QueueThresholdSignaller(QQueue<T>* queue, int threshold) 
        : queue(queue), threshold(threshold) {}

    int getThreshold() const { return threshold; }
    QWaitCondition& getBelowThresholdEvent() { return belowThresholdEvent; }

    void signalIfBelowThreshold() {
        if (queue && queue->size() < threshold) {
            belowThresholdEvent.wakeAll();
        }
    }

private:
    QQueue<T>* queue;
    int threshold;
    QWaitCondition belowThresholdEvent;
};

#endif // QUEUETHRESHOLDSIGNALLER_HPP
