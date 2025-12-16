#ifndef COMMANDS_H
#define COMMANDS_H

#include "command.h"
#include "route.h"
#include "trip.h"
#include "vehicle.h"
#include "stop.h"
#include "driver.h"

class TransportSystem;

class AddRouteCommand : public Command {
private:
    TransportSystem* system;
    std::shared_ptr<Route> route;

public:
    AddRouteCommand(TransportSystem* sys, std::shared_ptr<Route> r);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

class RemoveRouteCommand : public Command {
private:
    TransportSystem* system;
    std::shared_ptr<Route> route;
    int routeNumber;

public:
    RemoveRouteCommand(TransportSystem* sys, int num);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

class AddTripCommand : public Command {
private:
    TransportSystem* system;
    std::shared_ptr<Trip> trip;

public:
    AddTripCommand(TransportSystem* sys, std::shared_ptr<Trip> t);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

class RemoveTripCommand : public Command {
private:
    TransportSystem* system;
    std::shared_ptr<Trip> trip;
    int tripId;

public:
    RemoveTripCommand(TransportSystem* sys, int id);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

class AddVehicleCommand : public Command {
private:
    TransportSystem* system;
    std::shared_ptr<Vehicle> vehicle;

public:
    AddVehicleCommand(TransportSystem* sys, std::shared_ptr<Vehicle> v);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

class RemoveVehicleCommand : public Command {
private:
    TransportSystem* system;
    std::shared_ptr<Vehicle> vehicle;
    std::string licensePlate;

public:
    RemoveVehicleCommand(TransportSystem* sys, const std::string& lp);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

class AddStopCommand : public Command {
private:
    TransportSystem* system;
    Stop stop;

public:
    AddStopCommand(TransportSystem* sys, const Stop& s);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

class RemoveStopCommand : public Command {
private:
    TransportSystem* system;
    Stop stop;
    int stopId;

public:
    RemoveStopCommand(TransportSystem* sys, int id);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

class AddDriverCommand : public Command {
private:
    TransportSystem* system;
    std::shared_ptr<Driver> driver;

public:
    AddDriverCommand(TransportSystem* sys, std::shared_ptr<Driver> d);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

class RemoveDriverCommand : public Command {
private:
    TransportSystem* system;
    std::shared_ptr<Driver> driver;
    std::string firstName, lastName, middleName;

public:
    RemoveDriverCommand(TransportSystem* sys, std::shared_ptr<Driver> d);
    void execute() override;
    void undo() override;
    std::string getDescription() const override;
};

#endif // COMMANDS_H

