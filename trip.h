#ifndef TRIP_H
#define TRIP_H

#include <memory>
#include <map>
#include <string>
#include "route.h"
#include "vehicle.h"
#include "driver.h"
#include "time.h"
#include "exceptions.h"

class TransportSystem;

class Trip {
private:
    int tripId;
    std::shared_ptr<Route> route;
    std::shared_ptr<Vehicle> vehicle;
    std::shared_ptr<Driver> driver;
    Time startTime;
    std::map<std::string, Time> schedule; // остановка -> время прибытия
    int weekDay; // День недели: 1-понедельник, 2-вторник, ..., 7-воскресенье

public:
    Trip(int id, std::shared_ptr<Route> r, std::shared_ptr<Vehicle> v,
         std::shared_ptr<Driver> d, const Time& start, int day = 1);

    void setArrivalTime(const std::string& stop, const Time& time);
    Time getArrivalTime(const std::string& stop) const;
    bool hasStop(const std::string& stop) const;

    int getTripId() const;
    std::shared_ptr<Route> getRoute() const;
    std::shared_ptr<Vehicle> getVehicle() const;
    std::shared_ptr<Driver> getDriver() const;
    Time getStartTime() const;
    const std::map<std::string, Time>& getSchedule() const;
    int getWeekDay() const;

    Time getEstimatedEndTime() const;

    std::string serialize() const;
    static std::shared_ptr<Trip> deserialize(const std::string& data, TransportSystem* system = nullptr);
};

#endif // TRIP_H

