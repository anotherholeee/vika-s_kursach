#include "driver_schedule.h"

void DriverSchedule::assignTripToDriver(std::shared_ptr<Driver> driver,
                           std::shared_ptr<Trip> trip) {
    driverTrips[driver].push_back(trip);
}

void DriverSchedule::removeTripFromDriver(std::shared_ptr<Driver> driver, int tripId) {
    auto& trips = driverTrips[driver];
    trips.erase(std::remove_if(trips.begin(), trips.end(),
        [tripId](const auto& trip) {
            return trip->getTripId() == tripId;
        }), trips.end());
}

bool DriverSchedule::isDriverAvailable(std::shared_ptr<Driver> driver,
                          const Time& startTime,
                          const Time& endTime) const {
    auto it = driverTrips.find(driver);
    if (it == driverTrips.end()) return true;

    for (const auto& trip : it->second) {
        Time tripStart = trip->getStartTime();
        Time tripEnd = tripStart + 60;

        if (!(endTime < tripStart || startTime > tripEnd)) {
            return false;
        }
    }
    return true;
}

bool DriverSchedule::checkWorkingHoursCompliance(std::shared_ptr<Driver> driver) const {
    auto it = driverTrips.find(driver);
    if (it == driverTrips.end()) return true;

    int totalMinutes = 0;
    for (const auto& trip : it->second) {
        totalMinutes += 60;
    }

    return totalMinutes <= MAX_WORKING_HOURS;
}

std::vector<std::shared_ptr<Trip>> DriverSchedule::getDriverTrips(
    std::shared_ptr<Driver> driver) const {
    auto it = driverTrips.find(driver);
    if (it != driverTrips.end()) {
        return it->second;
    }
    return {};
}

int DriverSchedule::getTotalWorkingMinutes(std::shared_ptr<Driver> driver) const {
    auto trips = getDriverTrips(driver);
    return static_cast<int>(trips.size()) * 60;
}

