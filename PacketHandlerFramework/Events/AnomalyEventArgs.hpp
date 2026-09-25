#ifndef ANOMALYEVENTARGS_HPP
#define ANOMALYEVENTARGS_HPP

#include <QString>

class AnomalyEventArgs {
public:
    AnomalyEventArgs(const QString& message) : message(message) {}
    QString getMessage() const { return message; }

private:
    QString message;
};

#endif // ANOMALYEVENTARGS_HPP
