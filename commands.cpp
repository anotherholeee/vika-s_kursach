#include "commands.h"
#include "transport_system.h"

AddRouteCommand::AddRouteCommand(TransportSystem* sys, std::shared_ptr<Route> r)
    : system(sys), route(r) {}

void AddRouteCommand::execute() {
    system->addRouteDirect(route);
}

void AddRouteCommand::undo() {
    system->removeRouteDirect(route->getNumber());
}

std::string AddRouteCommand::getDescription() const {
    return "Добавление маршрута " + std::to_string(route->getNumber());
}

RemoveRouteCommand::RemoveRouteCommand(TransportSystem* sys, int num)
    : system(sys), routeNumber(num), route(nullptr) {}

void RemoveRouteCommand::execute() {
    route = system->getRouteByNumber(routeNumber);
    system->removeRouteDirect(routeNumber);
}

void RemoveRouteCommand::undo() {
    if (route) {
        system->addRouteDirect(route);
    }
}

std::string RemoveRouteCommand::getDescription() const {
    return "Удаление маршрута " + std::to_string(routeNumber);
}

AddTripCommand::AddTripCommand(TransportSystem* sys, std::shared_ptr<Trip> t)
    : system(sys), trip(t) {}

void AddTripCommand::execute() {
    system->addTripDirect(trip);
}

void AddTripCommand::undo() {
    system->removeTripDirect(trip->getTripId());
}

std::string AddTripCommand::getDescription() const {
    return "Добавление рейса " + std::to_string(trip->getTripId());
}

RemoveTripCommand::RemoveTripCommand(TransportSystem* sys, int id)
    : system(sys), tripId(id), trip(nullptr) {}

void RemoveTripCommand::execute() {
    trip = system->getTripById(tripId);
    system->removeTripDirect(tripId);
}

void RemoveTripCommand::undo() {
    if (trip) {
        system->addTripDirect(trip);
    }
}

std::string RemoveTripCommand::getDescription() const {
    return "Удаление рейса " + std::to_string(tripId);
}

AddVehicleCommand::AddVehicleCommand(TransportSystem* sys, std::shared_ptr<Vehicle> v)
    : system(sys), vehicle(v) {}

void AddVehicleCommand::execute() {
    system->addVehicleDirect(vehicle);
}

void AddVehicleCommand::undo() {
    system->removeVehicleDirect(vehicle->getLicensePlate());
}

std::string AddVehicleCommand::getDescription() const {
    return "Добавление транспорта " + vehicle->getLicensePlate();
}

RemoveVehicleCommand::RemoveVehicleCommand(TransportSystem* sys, const std::string& lp)
    : system(sys), licensePlate(lp), vehicle(nullptr) {}

void RemoveVehicleCommand::execute() {
    vehicle = system->getVehicleByLicensePlate(licensePlate);
    system->removeVehicleDirect(licensePlate);
}

void RemoveVehicleCommand::undo() {
    if (vehicle) {
        system->addVehicleDirect(vehicle);
    }
}

std::string RemoveVehicleCommand::getDescription() const {
    return "Удаление транспорта " + licensePlate;
}

AddStopCommand::AddStopCommand(TransportSystem* sys, const Stop& s)
    : system(sys), stop(s) {}

void AddStopCommand::execute() {
    system->addStopDirect(stop);
}

void AddStopCommand::undo() {
    system->removeStopDirect(stop.getId());
}

std::string AddStopCommand::getDescription() const {
    return "Добавление остановки " + stop.getName();
}

RemoveStopCommand::RemoveStopCommand(TransportSystem* sys, int id)
    : system(sys), stopId(id), stop(0, "") {}

void RemoveStopCommand::execute() {
    stop = system->getStopById(stopId);
    system->removeStopDirect(stopId);
}

void RemoveStopCommand::undo() {
    system->addStopDirect(stop);
}

std::string RemoveStopCommand::getDescription() const {
    return "Удаление остановки ID " + std::to_string(stopId);
}

AddDriverCommand::AddDriverCommand(TransportSystem* sys, std::shared_ptr<Driver> d)
    : system(sys), driver(d) {}

void AddDriverCommand::execute() {
    system->addDriverDirect(driver);
}

void AddDriverCommand::undo() {
    system->removeDriverDirect(driver);
}

std::string AddDriverCommand::getDescription() const {
    return "Добавление водителя " + driver->getFullName();
}

RemoveDriverCommand::RemoveDriverCommand(TransportSystem* sys, std::shared_ptr<Driver> d)
    : system(sys), driver(d),
      firstName(d->getFirstName()),
      lastName(d->getLastName()),
      middleName(d->getMiddleName()) {}

void RemoveDriverCommand::execute() {
    system->removeDriverDirect(driver);
}

void RemoveDriverCommand::undo() {
    system->addDriverDirect(driver);
}

std::string RemoveDriverCommand::getDescription() const {
    return "Удаление водителя " + driver->getFullName();
}

