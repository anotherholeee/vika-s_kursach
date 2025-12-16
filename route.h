#ifndef ROUTE_H
#define ROUTE_H

#include <vector>
#include <string>
#include <set>
#include <memory>
#include <algorithm>
#include "exceptions.h"

class Route {
private:
    int number;
    std::string vehicleType;
    std::string startStop;
    std::string endStop;
    std::vector<std::string> allStops;
    std::set<int> weekDays; // Дни недели: 1-понедельник, 2-вторник, ..., 7-воскресенье

public:
    Route(int num, const std::string& vType, const std::vector<std::string>& stops, 
          const std::set<int>& days = {1,2,3,4,5,6,7});

    bool containsStop(const std::string& stop) const;
    int getStopPosition(const std::string& stop) const;
    bool isStopBefore(const std::string& stopA, const std::string& stopB) const;

    int getNumber() const;
    std::string getVehicleType() const;
    std::string getStartStop() const;
    std::string getEndStop() const;
    const std::vector<std::string>& getAllStops() const;
    const std::set<int>& getWeekDays() const;
    bool operatesOnDay(int day) const;

    std::string serialize() const;
    static std::shared_ptr<Route> deserialize(const std::string& data);
};

#endif // ROUTE_H

