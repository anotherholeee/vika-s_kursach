#ifndef JOURNEY_H
#define JOURNEY_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include "trip.h"
#include "time.h"

class Journey {
private:
    std::vector<std::shared_ptr<Trip>> trips;
    std::vector<std::string> transferPoints;
    Time startTime;
    Time endTime;
    int transferCount;

public:
    Journey(const std::vector<std::shared_ptr<Trip>>& tripList,
            const std::vector<std::string>& transfers,
            Time start, Time end);

    int getTotalDuration() const;
    int getTransferCount() const;
    Time getStartTime() const;
    Time getEndTime() const;
    const std::vector<std::shared_ptr<Trip>>& getTrips() const;
    const std::vector<std::string>& getTransferPoints() const;

    void display() const;
};

#endif // JOURNEY_H

