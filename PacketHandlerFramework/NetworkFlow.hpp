#ifndef NETWORKFLOW_HPP
#define NETWORKFLOW_HPP

#include "FiveTuple.hpp"

class NetworkFlow {
public:
    NetworkFlow(const FiveTuple& fiveTuple) : fiveTuple(fiveTuple) {}
    FiveTuple getFiveTuple() const { return fiveTuple; }

private:
    FiveTuple fiveTuple;
};

#endif // NETWORKFLOW_HPP
