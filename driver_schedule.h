#ifndef DRIVER_SCHEDULE_H
#define DRIVER_SCHEDULE_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include "driver.h"
#include "trip.h"
#include "time.h"

class DriverSchedule {
private:
    std::unordered_map<std::shared_ptr<Driver>,
                       std::vector<std::shared_ptr<Trip>>> driverTrips;
    const int MAX_WORKING_HOURS = 12 * 60; // 12 часов в минутах

public:
    void assignTripToDriver(std::shared_ptr<Driver> driver,
                           std::shared_ptr<Trip> trip);

    void removeTripFromDriver(std::shared_ptr<Driver> driver, int tripId);

    bool isDriverAvailable(std::shared_ptr<Driver> driver,
                          const Time& startTime,
                          const Time& endTime) const;

    bool checkWorkingHoursCompliance(std::shared_ptr<Driver> driver) const;

    std::vector<std::shared_ptr<Trip>> getDriverTrips(
        std::shared_ptr<Driver> driver) const;

    int getTotalWorkingMinutes(std::shared_ptr<Driver> driver) const;
};

#endif // DRIVER_SCHEDULE_H

