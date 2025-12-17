#include "trip.h"
#include "transport_system.h"
#include "bus.h"
#include "tram.h"
#include "trolleybus.h"
#include <sstream>

Trip::Trip(int id, std::shared_ptr<Route> r, std::shared_ptr<Vehicle> v,
         std::shared_ptr<Driver> d, const Time& start, int day)
    : tripId(id), route(std::move(r)), vehicle(std::move(v)),
      driver(std::move(d)), startTime(start), weekDay(day) {
    if (day < 1 || day > 7) {
        throw InputException("День недели должен быть от 1 до 7");
    }
}

void Trip::setArrivalTime(const std::string& stop, const Time& time) {
    schedule[stop] = time;
}

Time Trip::getArrivalTime(const std::string& stop) const {
    auto it = schedule.find(stop);
    if (it != schedule.end()) {
        return it->second;
    }
    throw ContainerException("Остановка не найдена в расписании рейса");
}

bool Trip::hasStop(const std::string& stop) const {
    return schedule.find(stop) != schedule.end();
}

int Trip::getTripId() const {
    return tripId;
}

std::shared_ptr<Route> Trip::getRoute() const {
    return route;
}

std::shared_ptr<Vehicle> Trip::getVehicle() const {
    return vehicle;
}

std::shared_ptr<Driver> Trip::getDriver() const {
    return driver;
}

Time Trip::getStartTime() const {
    return startTime;
}

const std::map<std::string, Time>& Trip::getSchedule() const {
    return schedule;
}

int Trip::getWeekDay() const {
    return weekDay;
}

Time Trip::getEstimatedEndTime() const {
    return startTime + 60;
}

std::string Trip::serialize() const {
    std::string result = std::to_string(tripId) + "|" + route->serialize() + "|" +
                       vehicle->serialize() + "|" + driver->serialize() + "|" +
                       startTime.serialize() + "|" + std::to_string(weekDay) + "|";

    std::string scheduleStr;
    for (const auto& [stop, time] : schedule) {
        scheduleStr += stop + "=" + time.serialize() + ";";
    }
    if (!scheduleStr.empty()) scheduleStr.pop_back();
    result += scheduleStr;

    return result;
}

std::shared_ptr<Trip> Trip::deserialize(const std::string& data, TransportSystem* system) {
    std::istringstream ss(data);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 6) {
        throw InputException("Некорректные данные рейса");
    }

    int tripId = std::stoi(tokens[0]);

    std::shared_ptr<Route> route;
    int vehicleTokenIndex, driverTokenIndex, timeTokenIndex, scheduleTokenIndex;

    if (tokens.size() >= 11) {
        std::string routeData = tokens[1] + "|" + tokens[2] + "|" + tokens[3];
        route = Route::deserialize(routeData);
        vehicleTokenIndex = 4;
        driverTokenIndex = 7;
        timeTokenIndex = 10;
        scheduleTokenIndex = 11;
    } else {
        route = Route::deserialize(tokens[1]);
        vehicleTokenIndex = 2;
        driverTokenIndex = 3;
        timeTokenIndex = 4;
        scheduleTokenIndex = 5;
    }

    std::shared_ptr<Vehicle> vehicle = nullptr;
    if (system) {
        std::string type, model, licensePlate;

        if (tokens.size() >= 11) {
            type = tokens[vehicleTokenIndex];
            model = tokens[vehicleTokenIndex + 1];
            licensePlate = tokens[vehicleTokenIndex + 2];
        } else {
            std::istringstream vehicleStream(tokens[vehicleTokenIndex]);
            std::getline(vehicleStream, type, '|');
            std::getline(vehicleStream, model, '|');
            std::getline(vehicleStream, licensePlate);
        }

        vehicle = system->findVehicleByLicensePlate(licensePlate);

        if (!vehicle && tokens.size() >= 11) {
            std::shared_ptr<Vehicle> newVehicle;
            if (type == "Автобус") {
                newVehicle = std::make_shared<Bus>(model, licensePlate);
            } else if (type == "Трамвай") {
                newVehicle = std::make_shared<Tram>(model, licensePlate);
            } else if (type == "Троллейбус") {
                newVehicle = std::make_shared<Trolleybus>(model, licensePlate);
            }
            if (newVehicle) {
                try {
                    system->addVehicle(newVehicle);
                    vehicle = newVehicle;
                } catch (...) {
                    vehicle = system->findVehicleByLicensePlate(licensePlate);
                }
            }
        }
    }

    std::shared_ptr<Driver> driver;
    if (tokens.size() >= 11) {
        std::string firstName = tokens[driverTokenIndex];
        std::string lastName = tokens[driverTokenIndex + 1];
        std::string middleName = (driverTokenIndex + 2 < tokens.size() && !tokens[driverTokenIndex + 2].empty()) ? tokens[driverTokenIndex + 2] : "";

        driver = system ? system->findDriverByName(firstName, lastName, middleName) : nullptr;

        if (!driver) {
            driver = std::make_shared<Driver>(firstName, lastName, middleName);
            if (system) {
                try {
                    system->addDriver(driver);
                } catch (...) {
                    driver = system->findDriverByName(firstName, lastName, middleName);
                }
            }
        }
    } else {
        driver = Driver::deserialize(tokens[driverTokenIndex]);
        if (system) {
            auto existingDriver = system->findDriverByName(
                driver->getFirstName(),
                driver->getLastName(),
                driver->getMiddleName());
            if (existingDriver) {
                driver = existingDriver;
            } else {
                try {
                    system->addDriver(driver);
                } catch (...) {
                }
            }
        }
    }

    Time startTime = Time::deserialize(tokens[timeTokenIndex]);
    
    int weekDay = 1;
    if (tokens.size() < 11) {
        if (timeTokenIndex + 1 < tokens.size()) {
            try {
                weekDay = std::stoi(tokens[timeTokenIndex + 1]);
                if (weekDay < 1 || weekDay > 7) weekDay = 1;
                scheduleTokenIndex = timeTokenIndex + 2;
            } catch (...) {
                scheduleTokenIndex = timeTokenIndex + 1;
            }
        } else {
            scheduleTokenIndex = timeTokenIndex + 1;
        }
    } else {
        scheduleTokenIndex = timeTokenIndex + 1;
    }

    if (!vehicle) {
        throw ContainerException("Транспортное средство не найдено в системе");
    }

    auto trip = std::make_shared<Trip>(tripId, route, vehicle, driver, startTime, weekDay);

    if (scheduleTokenIndex < tokens.size() && !tokens[scheduleTokenIndex].empty()) {
        std::istringstream scheduleStream(tokens[scheduleTokenIndex]);
        std::string stopTimePair;
        while (std::getline(scheduleStream, stopTimePair, ';')) {
            size_t eqPos = stopTimePair.find('=');
            if (eqPos != std::string::npos) {
                std::string stop = stopTimePair.substr(0, eqPos);
                std::string timeStr = stopTimePair.substr(eqPos + 1);
                trip->setArrivalTime(stop, Time::deserialize(timeStr));
            }
        }
    }

    return trip;
}

