#include "route.h"
#include <sstream>

Route::Route(int num, const std::string& vType, const std::vector<std::string>& stops, 
          const std::set<int>& days)
    : number(num), vehicleType(vType), allStops(stops), weekDays(days) {
    if (stops.empty()) {
        throw ContainerException("Маршрут не может быть пустым");
    }
    startStop = stops.front();
    endStop = stops.back();
}

bool Route::containsStop(const std::string& stop) const {
    return std::find(allStops.begin(), allStops.end(), stop) != allStops.end();
}

int Route::getStopPosition(const std::string& stop) const {
    auto it = std::find(allStops.begin(), allStops.end(), stop);
    if (it != allStops.end()) {
        return static_cast<int>(std::distance(allStops.begin(), it));
    }
    return -1;
}

bool Route::isStopBefore(const std::string& stopA, const std::string& stopB) const {
    int posA = getStopPosition(stopA);
    int posB = getStopPosition(stopB);
    return posA != -1 && posB != -1 && posA < posB;
}

int Route::getNumber() const {
    return number;
}

std::string Route::getVehicleType() const {
    return vehicleType;
}

std::string Route::getStartStop() const {
    return startStop;
}

std::string Route::getEndStop() const {
    return endStop;
}

const std::vector<std::string>& Route::getAllStops() const {
    return allStops;
}

const std::set<int>& Route::getWeekDays() const {
    return weekDays;
}

bool Route::operatesOnDay(int day) const {
    return weekDays.find(day) != weekDays.end();
}

std::string Route::serialize() const {
    std::string result = std::to_string(number) + "|" + vehicleType + "|";
    for (size_t i = 0; i < allStops.size(); ++i) {
        result += allStops[i];
        if (i < allStops.size() - 1) result += ";";
    }
    result += "|";
    for (auto it = weekDays.begin(); it != weekDays.end(); ++it) {
        result += std::to_string(*it);
        if (std::next(it) != weekDays.end()) result += ",";
    }
    return result;
}

std::shared_ptr<Route> Route::deserialize(const std::string& data) {
    std::istringstream ss(data);
    std::string numberStr, vehicleType, stopsStr, daysStr;
    std::getline(ss, numberStr, '|');
    std::getline(ss, vehicleType, '|');
    std::getline(ss, stopsStr, '|');
    std::getline(ss, daysStr);

    std::vector<std::string> stops;
    std::istringstream stopsStream(stopsStr);
    std::string stop;
    while (std::getline(stopsStream, stop, ';')) {
        stops.push_back(stop);
    }

    std::set<int> weekDays;
    if (!daysStr.empty()) {
        std::istringstream daysStream(daysStr);
        std::string day;
        while (std::getline(daysStream, day, ',')) {
            weekDays.insert(std::stoi(day));
        }
    } else {
        weekDays = {1,2,3,4,5,6,7};
    }

    return std::make_shared<Route>(std::stoi(numberStr), vehicleType, stops, weekDays);
}

