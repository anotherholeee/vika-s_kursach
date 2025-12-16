#include "transport_system.h"
#include <iostream>
#include <algorithm>

TransportSystem::TransportSystem() 
    : journeyPlanner(this), 
      dataManager(),
      arrivalTimeAlgorithm(std::make_unique<ArrivalTimeCalculationAlgorithm>(this)),
      routeSearchAlgorithm(std::make_unique<RouteSearchAlgorithm>(this)) {
    adminCredentials["admin"] = "admin123";
    adminCredentials["manager"] = "manager123";
}

bool TransportSystem::canUndo() const {
    return commandHistory.canUndo();
}

void TransportSystem::undo() {
    if (!canUndo()) {
        throw ContainerException("Нет действий для отмены");
    }
    commandHistory.undo();
    std::cout << "Действие отменено.\n";
}

std::string TransportSystem::getLastCommandDescription() const {
    return commandHistory.getLastCommandDescription();
}

bool TransportSystem::authenticateAdmin(const std::string& username, const std::string& password) {
    auto it = adminCredentials.find(username);
    return it != adminCredentials.end() && it->second == password;
}

void TransportSystem::addAdmin(const std::string& username, const std::string& password) {
    adminCredentials[username] = password;
}

const std::unordered_map<std::string, std::string>& TransportSystem::getAdminCredentials() const {
    return adminCredentials;
}

void TransportSystem::setAdminCredentials(const std::unordered_map<std::string, std::string>& creds) {
    adminCredentials = creds;
}

void TransportSystem::saveData() {
    dataManager.saveAllData(*this);
}

void TransportSystem::loadData() {
    dataManager.loadAllData(*this);
}

std::vector<std::shared_ptr<Route>> TransportSystem::findRoutes(const std::string& stopA, const std::string& stopB) {
    // Используем алгоритм поиска маршрутов
    return routeSearchAlgorithm->findRoutes(stopA, stopB);
}

void TransportSystem::getStopTimetable(int stopId, const Time& startTime, const Time& endTime) {
    auto it = stopIdToName.find(stopId);
    if (it == stopIdToName.end()) {
        throw ContainerException("Остановка с ID " + std::to_string(stopId) + " не найдена");
    }
    const std::string& stopName = it->second;

    std::vector<std::pair<int, Time>> relevantTrips;

    for (const auto& trip : trips) {
        if (trip->hasStop(stopName)) {
            Time arrivalTime = trip->getArrivalTime(stopName);
            if (startTime <= arrivalTime && arrivalTime <= endTime) {
                relevantTrips.push_back({trip->getRoute()->getNumber(), arrivalTime});
            }
        }
    }

    std::sort(relevantTrips.begin(), relevantTrips.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    std::cout << "\nРасписание для остановки '" << stopName << "' с "
              << startTime << " по " << endTime << ":\n";
    if (relevantTrips.empty()) {
        std::cout << "Рейсов не найдено.\n";
    } else {
        for (const auto& trip : relevantTrips) {
            std::cout << "Маршрут " << trip.first << " - прибытие в " << trip.second << '\n';
        }
    }
}

void TransportSystem::getStopTimetableAll(const std::string& stopName) {
    std::vector<std::pair<std::pair<int, std::string>, Time>> relevantTrips; // routeNumber, vehicleType, time

    for (const auto& trip : trips) {
        if (trip->hasStop(stopName)) {
            Time arrivalTime = trip->getArrivalTime(stopName);
            relevantTrips.push_back({{trip->getRoute()->getNumber(), trip->getRoute()->getVehicleType()}, arrivalTime});
        }
    }

    std::sort(relevantTrips.begin(), relevantTrips.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    std::cout << "\nРасписание для остановки '" << stopName << "':\n";
    if (relevantTrips.empty()) {
        std::cout << "Рейсов не найдено.\n";
    } else {
        for (const auto& trip : relevantTrips) {
            std::cout << trip.first.second << " " << trip.first.first << " - прибытие в " << trip.second << '\n';
        }
    }
}

void TransportSystem::calculateArrivalTimes(int tripId, double averageSpeed) {
    // Используем алгоритм расчета времени прибытия
    arrivalTimeAlgorithm->calculateArrivalTimes(tripId, averageSpeed);
}

ArrivalTimeCalculationAlgorithm* TransportSystem::getArrivalTimeAlgorithm() const {
    return arrivalTimeAlgorithm.get();
}

RouteSearchAlgorithm* TransportSystem::getRouteSearchAlgorithm() const {
    return routeSearchAlgorithm.get();
}

void TransportSystem::addRoute(std::shared_ptr<Route> route) {
    for (const auto& existingRoute : routes) {
        if (existingRoute->getNumber() == route->getNumber()) {
            throw ContainerException("Маршрут с номером " + std::to_string(route->getNumber()) + " уже существует");
        }
    }
    commandHistory.executeCommand(std::make_unique<AddRouteCommand>(this, route));
}

void TransportSystem::addTrip(std::shared_ptr<Trip> trip) {
    for (const auto& existingTrip : trips) {
        if (existingTrip->getTripId() == trip->getTripId()) {
            throw ContainerException("Рейс с ID " + std::to_string(trip->getTripId()) + " уже существует");
        }
    }

    bool driverExists = false;
    const auto& tripDriver = trip->getDriver();
    for (const auto& existingDriver : drivers) {
        if (existingDriver->getFirstName() == tripDriver->getFirstName() &&
            existingDriver->getLastName() == tripDriver->getLastName() &&
            existingDriver->getMiddleName() == tripDriver->getMiddleName()) {
            driverExists = true;
            break;
        }
    }
    if (!driverExists) {
        addDriver(trip->getDriver());
    }

    bool vehicleExists = false;
    const auto& tripVehicle = trip->getVehicle();
    for (const auto& existingVehicle : vehicles) {
        if (existingVehicle->getLicensePlate() == tripVehicle->getLicensePlate()) {
            vehicleExists = true;
            break;
        }
    }
    if (!vehicleExists) {
        addVehicle(trip->getVehicle());
    }

    commandHistory.executeCommand(std::make_unique<AddTripCommand>(this, trip));
}

void TransportSystem::addVehicle(std::shared_ptr<Vehicle> vehicle) {
    for (const auto& existingVehicle : vehicles) {
        if (existingVehicle->getLicensePlate() == vehicle->getLicensePlate()) {
            throw ContainerException("Транспортное средство с номером " + vehicle->getLicensePlate() + " уже существует");
        }
    }
    commandHistory.executeCommand(std::make_unique<AddVehicleCommand>(this, vehicle));
}

void TransportSystem::addDriver(std::shared_ptr<Driver> driver) {
    commandHistory.executeCommand(std::make_unique<AddDriverCommand>(this, driver));
}

void TransportSystem::addStop(const Stop& stop) {
    for (const auto& existingStop : stops) {
        if (existingStop.getId() == stop.getId()) {
            throw ContainerException("Остановка с ID " + std::to_string(stop.getId()) + " уже существует");
        }
    }
    commandHistory.executeCommand(std::make_unique<AddStopCommand>(this, stop));
}

void TransportSystem::removeRoute(int routeNumber) {
    auto it = std::find_if(routes.begin(), routes.end(),
                          [routeNumber](const auto& r) { return r->getNumber() == routeNumber; });
    if (it == routes.end()) {
        throw ContainerException("Маршрут с номером " + std::to_string(routeNumber) + " не найден");
    }
    commandHistory.executeCommand(std::make_unique<RemoveRouteCommand>(this, routeNumber));
    std::cout << "Маршрут " << routeNumber << " удален.\n";
}

void TransportSystem::removeTrip(int tripId) {
    auto it = std::find_if(trips.begin(), trips.end(),
                          [tripId](const auto& t) { return t->getTripId() == tripId; });
    if (it == trips.end()) {
        throw ContainerException("Рейс с ID " + std::to_string(tripId) + " не найден");
    }
    commandHistory.executeCommand(std::make_unique<RemoveTripCommand>(this, tripId));
    std::cout << "Рейс " << tripId << " удален.\n";
}

void TransportSystem::displayAllRoutes() const {
    std::cout << "\n=== ВСЕ МАРШРУТЫ ===\n";
    for (const auto& route : routes) {
        std::cout << "Маршрут " << route->getNumber() << " (" << route->getVehicleType()
                  << "): " << route->getStartStop() << " -> " << route->getEndStop() << "\n";
    }
}

void TransportSystem::displayAllTrips() const {
    std::cout << "\n=== ВСЕ РЕЙСЫ ===\n";
    for (const auto& trip : trips) {
        std::cout << "Рейс " << trip->getTripId() << ": Маршрут " << trip->getRoute()->getNumber()
                  << ", ТС: " << trip->getVehicle()->getInfo()
                  << ", Водитель: " << trip->getDriver()->getFullName()
                  << ", Отправление: " << trip->getStartTime() << "\n";
    }
}

void TransportSystem::displayAllVehicles() const {
    std::cout << "\n=== ВСЕ ТРАНСПОРТНЫЕ СРЕДСТВА ===\n";
    for (const auto& vehicle : vehicles) {
        std::cout << vehicle->getInfo() << '\n';
    }
}

void TransportSystem::displayAllStops() const {
    std::cout << "\n=== ВСЕ ОСТАНОВКИ ===\n";
    for (const auto& stop : stops) {
        std::cout << "ID: " << stop.getId() << " - " << stop.getName() << '\n';
    }
}

void TransportSystem::displayAllDrivers() const {
    std::cout << "\n=== ВСЕ ВОДИТЕЛИ ===\n";
    for (const auto& driver : drivers) {
        std::cout << driver->getFullName() << '\n';
    }
}

const std::vector<std::shared_ptr<Trip>>& TransportSystem::getTrips() const {
    return trips;
}

const std::vector<std::shared_ptr<Route>>& TransportSystem::getRoutes() const {
    return routes;
}

const std::vector<std::shared_ptr<Vehicle>>& TransportSystem::getVehicles() const {
    return vehicles;
}

const DynamicArray<Stop>& TransportSystem::getStops() const {
    return stops;
}

const std::vector<std::shared_ptr<Driver>>& TransportSystem::getDrivers() const {
    return drivers;
}

JourneyPlanner& TransportSystem::getJourneyPlanner() {
    return journeyPlanner;
}

DriverSchedule& TransportSystem::getDriverSchedule() {
    return driverSchedule;
}

std::shared_ptr<Driver> TransportSystem::findDriverByName(const std::string& firstName,
                                        const std::string& lastName,
                                        const std::string& middleName) const {
    for (const auto& driver : drivers) {
        if (driver->getFirstName() == firstName &&
            driver->getLastName() == lastName &&
            (middleName.empty() || driver->getMiddleName() == middleName)) {
            return driver;
        }
    }
    return nullptr;
}

std::shared_ptr<Vehicle> TransportSystem::findVehicleByLicensePlate(const std::string& licensePlate) const {
    for (const auto& vehicle : vehicles) {
        if (vehicle->getLicensePlate() == licensePlate) {
            return vehicle;
        }
    }
    return nullptr;
}

std::shared_ptr<Route> TransportSystem::findRouteByNumber(int number) const {
    for (const auto& route : routes) {
        if (route->getNumber() == number) {
            return route;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Trip>> TransportSystem::getTripsThroughStop(const std::string& stopName) const {
    std::vector<std::shared_ptr<Trip>> result;
    for (const auto& trip : trips) {
        if (trip->hasStop(stopName)) {
            result.push_back(trip);
        }
    }
    return result;
}

std::string TransportSystem::getStopNameById(int id) const {
    auto it = stopIdToName.find(id);
    if (it != stopIdToName.end()) {
        return it->second;
    }
    return "";
}

std::shared_ptr<Route> TransportSystem::getRouteByNumber(int number) {
    for (const auto& route : routes) {
        if (route->getNumber() == number) {
            return route;
        }
    }
    return nullptr;
}

std::shared_ptr<Trip> TransportSystem::getTripById(int id) {
    for (const auto& trip : trips) {
        if (trip->getTripId() == id) {
            return trip;
        }
    }
    return nullptr;
}

std::shared_ptr<Vehicle> TransportSystem::getVehicleByLicensePlate(const std::string& licensePlate) {
    for (const auto& vehicle : vehicles) {
        if (vehicle->getLicensePlate() == licensePlate) {
            return vehicle;
        }
    }
    return nullptr;
}

Stop TransportSystem::getStopById(int id) {
    for (const auto& stop : stops) {
        if (stop.getId() == id) {
            return stop;
        }
    }
    throw ContainerException("Остановка с ID " + std::to_string(id) + " не найдена");
}

void TransportSystem::addRouteDirect(std::shared_ptr<Route> route) {
    routes.push_back(std::move(route));
}

void TransportSystem::removeRouteDirect(int routeNumber) {
    auto it = std::find_if(routes.begin(), routes.end(),
                          [routeNumber](const auto& r) { return r->getNumber() == routeNumber; });
    if (it != routes.end()) {
        routes.erase(it);
    }
}

void TransportSystem::addTripDirect(std::shared_ptr<Trip> trip) {
    trips.push_back(std::move(trip));
}

void TransportSystem::removeTripDirect(int tripId) {
    auto it = std::find_if(trips.begin(), trips.end(),
                          [tripId](const auto& t) { return t->getTripId() == tripId; });
    if (it != trips.end()) {
        trips.erase(it);
    }
}

void TransportSystem::addVehicleDirect(std::shared_ptr<Vehicle> vehicle) {
    vehicles.push_back(std::move(vehicle));
}

void TransportSystem::removeVehicleDirect(const std::string& licensePlate) {
    auto it = std::find_if(vehicles.begin(), vehicles.end(),
                          [&licensePlate](const auto& v) {
                              return v->getLicensePlate() == licensePlate;
                          });
    if (it != vehicles.end()) {
        vehicles.erase(it);
    }
}

void TransportSystem::addStopDirect(const Stop& stop) {
    stops.push_back(stop);
    stopIdToName[stop.getId()] = stop.getName();
}

void TransportSystem::removeStopDirect(int stopId) {
    auto it = std::find_if(stops.begin(), stops.end(),
                          [stopId](const auto& s) { return s.getId() == stopId; });
    if (it != stops.end()) {
        stopIdToName.erase(stopId);
        stops.erase(it);
    }
}

void TransportSystem::addDriverDirect(std::shared_ptr<Driver> driver) {
    drivers.push_back(std::move(driver));
}

void TransportSystem::removeDriverDirect(std::shared_ptr<Driver> driver) {
    auto it = std::find_if(drivers.begin(), drivers.end(),
                          [&driver](const auto& d) {
                              return d->getFirstName() == driver->getFirstName() &&
                                     d->getLastName() == driver->getLastName() &&
                                     d->getMiddleName() == driver->getMiddleName();
                          });
    if (it != drivers.end()) {
        drivers.erase(it);
    }
}

