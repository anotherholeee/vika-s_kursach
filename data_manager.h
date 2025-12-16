#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <string>
#include <filesystem>

class TransportSystem;

class DataManager {
private:
    std::string dataDirectory;

public:
    DataManager(const std::string& dir = "data/");

    void saveAllData(TransportSystem& system);
    void loadAllData(TransportSystem& system);

private:
    void saveStops(TransportSystem& system);
    void saveVehicles(TransportSystem& system);
    void saveDrivers(TransportSystem& system);
    void saveRoutes(TransportSystem& system);
    void saveTrips(TransportSystem& system);
    void saveAdminCredentials(TransportSystem& system);

    void loadStops(TransportSystem& system);
    void loadVehicles(TransportSystem& system);
    void loadDrivers(TransportSystem& system);
    void loadRoutes(TransportSystem& system);
    void loadTrips(TransportSystem& system);
    void loadAdminCredentials(TransportSystem& system);
};

#endif // DATA_MANAGER_H

