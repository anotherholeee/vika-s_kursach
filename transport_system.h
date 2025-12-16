#ifndef TRANSPORT_SYSTEM_H
#define TRANSPORT_SYSTEM_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "dynamic_array.h"
#include "stop.h"
#include "route.h"
#include "trip.h"
#include "vehicle.h"
#include "driver.h"
#include "time.h"
#include "journey_planner.h"
#include "driver_schedule.h"
#include "data_manager.h"
#include "command.h"
#include "commands.h"
#include "algorithm.h"
#include "exceptions.h"
#include <iostream>
#include <algorithm>
#include <memory>

class TransportSystem {
private:
    std::vector<std::shared_ptr<Route>> routes;
    std::vector<std::shared_ptr<Trip>> trips;
    std::vector<std::shared_ptr<Vehicle>> vehicles;
    std::vector<std::shared_ptr<Driver>> drivers;
    DynamicArray<Stop> stops;
    std::unordered_map<int, std::string> stopIdToName;
    std::unordered_map<std::string, std::string> adminCredentials;

    JourneyPlanner journeyPlanner;
    DriverSchedule driverSchedule;
    DataManager dataManager;
    CommandHistory commandHistory;
    
    // Алгоритмы (Strategy pattern)
    std::unique_ptr<ArrivalTimeCalculationAlgorithm> arrivalTimeAlgorithm;
    std::unique_ptr<RouteSearchAlgorithm> routeSearchAlgorithm;

public:
    TransportSystem();

    bool canUndo() const;
    void undo();
    std::string getLastCommandDescription() const;

    bool authenticateAdmin(const std::string& username, const std::string& password);
    void addAdmin(const std::string& username, const std::string& password);
    const std::unordered_map<std::string, std::string>& getAdminCredentials() const;
    void setAdminCredentials(const std::unordered_map<std::string, std::string>& creds);

    void saveData();
    void loadData();

    std::vector<std::shared_ptr<Route>> findRoutes(const std::string& stopA, const std::string& stopB);
    void getStopTimetable(int stopId, const Time& startTime, const Time& endTime);
    void getStopTimetableAll(const std::string& stopName);
    void calculateArrivalTimes(int tripId, double averageSpeed);
    
    // Получение алгоритмов
    ArrivalTimeCalculationAlgorithm* getArrivalTimeAlgorithm() const;
    RouteSearchAlgorithm* getRouteSearchAlgorithm() const;

    void addRoute(std::shared_ptr<Route> route);
    void addTrip(std::shared_ptr<Trip> trip);
    void addVehicle(std::shared_ptr<Vehicle> vehicle);
    void addDriver(std::shared_ptr<Driver> driver);
    void addStop(const Stop& stop);
    void removeRoute(int routeNumber);
    void removeTrip(int tripId);

    void displayAllRoutes() const;
    void displayAllTrips() const;
    void displayAllVehicles() const;
    void displayAllStops() const;
    void displayAllDrivers() const;

    const std::vector<std::shared_ptr<Trip>>& getTrips() const;
    const std::vector<std::shared_ptr<Route>>& getRoutes() const;
    const std::vector<std::shared_ptr<Vehicle>>& getVehicles() const;
    const DynamicArray<Stop>& getStops() const;
    const std::vector<std::shared_ptr<Driver>>& getDrivers() const;

    JourneyPlanner& getJourneyPlanner();
    DriverSchedule& getDriverSchedule();

    std::shared_ptr<Driver> findDriverByName(const std::string& firstName,
                                            const std::string& lastName,
                                            const std::string& middleName = "") const;
    std::shared_ptr<Vehicle> findVehicleByLicensePlate(const std::string& licensePlate) const;
    std::shared_ptr<Route> findRouteByNumber(int number) const;
    std::vector<std::shared_ptr<Trip>> getTripsThroughStop(const std::string& stopName) const;
    std::string getStopNameById(int id) const;

    std::shared_ptr<Route> getRouteByNumber(int number);
    std::shared_ptr<Trip> getTripById(int id);
    std::shared_ptr<Vehicle> getVehicleByLicensePlate(const std::string& licensePlate);
    Stop getStopById(int id);

    void addRouteDirect(std::shared_ptr<Route> route);
    void removeRouteDirect(int routeNumber);
    void addTripDirect(std::shared_ptr<Trip> trip);
    void removeTripDirect(int tripId);
    void addVehicleDirect(std::shared_ptr<Vehicle> vehicle);
    void removeVehicleDirect(const std::string& licensePlate);
    void addStopDirect(const Stop& stop);
    void removeStopDirect(int stopId);
    void addDriverDirect(std::shared_ptr<Driver> driver);
    void removeDriverDirect(std::shared_ptr<Driver> driver);

    friend class Trip;
};

#endif // TRANSPORT_SYSTEM_H

