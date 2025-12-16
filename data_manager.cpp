#include "data_manager.h"
#include "transport_system.h"
#include "stop.h"
#include "vehicle.h"
#include "bus.h"
#include "tram.h"
#include "trolleybus.h"
#include "driver.h"
#include "route.h"
#include "trip.h"
#include "exceptions.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

DataManager::DataManager(const std::string& dir) : dataDirectory(dir) {
    std::filesystem::create_directories(dataDirectory);
}

void DataManager::saveAllData(TransportSystem& system) {
    try {
        std::cout << "Сохранение данных в файлы...\n";

        saveStops(system);
        saveVehicles(system);
        saveDrivers(system);
        saveRoutes(system);
        saveTrips(system);
        saveAdminCredentials(system);

        std::cout << "Данные успешно сохранены!\n";
    } catch (const std::exception& e) {
        std::cout << "Ошибка при сохранении данных: " << e.what() << "\n";
    }
}

void DataManager::loadAllData(TransportSystem& system) {
    try {
        loadStops(system);
        loadVehicles(system);
        loadDrivers(system);
        loadRoutes(system);
        loadTrips(system);
        loadAdminCredentials(system);
    } catch (const std::exception& e) {
        // Ошибки обрабатываются в отдельных функциях загрузки
    }
}

void DataManager::saveStops(TransportSystem& system) {
    std::ofstream file(dataDirectory + "stops.txt");
    if (!file.is_open()) throw FileException("stops.txt", "открытие для записи");

    const auto& stops = system.getStops();
    for (const auto& stop : stops) {
        file << stop.serialize() << "\n";
    }
    file.close();
}

void DataManager::saveVehicles(TransportSystem& system) {
    std::ofstream file(dataDirectory + "vehicles.txt");
    if (!file.is_open()) throw FileException("vehicles.txt", "открытие для записи");

    const auto& vehicles = system.getVehicles();
    for (const auto& vehicle : vehicles) {
        file << vehicle->serialize() << "\n";
    }
    file.close();
}

void DataManager::saveDrivers(TransportSystem& system) {
    std::ofstream file(dataDirectory + "drivers.txt");
    if (!file.is_open()) throw FileException("drivers.txt", "открытие для записи");

    const auto& drivers = system.getDrivers();
    for (const auto& driver : drivers) {
        file << driver->serialize() << "\n";
    }
    file.close();
}

void DataManager::saveRoutes(TransportSystem& system) {
    std::ofstream file(dataDirectory + "routes.txt");
    if (!file.is_open()) throw FileException("routes.txt", "открытие для записи");

    const auto& routes = system.getRoutes();
    for (const auto& route : routes) {
        file << route->serialize() << "\n";
    }
    file.close();
}

void DataManager::saveTrips(TransportSystem& system) {
    std::ofstream file(dataDirectory + "trips.txt");
    if (!file.is_open()) throw FileException("trips.txt", "открытие для записи");

    const auto& trips = system.getTrips();
    for (const auto& trip : trips) {
        file << trip->serialize() << "\n";
    }
    file.close();
}

void DataManager::saveAdminCredentials(TransportSystem& system) {
    std::ofstream file(dataDirectory + "admins.bin", std::ios::binary);
    if (!file.is_open()) throw FileException("admins.bin", "открытие для записи");

    const auto& creds = system.getAdminCredentials();
    size_t count = creds.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [username, password] : creds) {
        size_t usernameLen = username.size();
        size_t passwordLen = password.size();
        file.write(reinterpret_cast<const char*>(&usernameLen), sizeof(usernameLen));
        file.write(username.c_str(), usernameLen);
        file.write(reinterpret_cast<const char*>(&passwordLen), sizeof(passwordLen));
        file.write(password.c_str(), passwordLen);
    }
    file.close();
}

void DataManager::loadStops(TransportSystem& system) {
    std::ifstream file(dataDirectory + "stops.txt");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        if (!line.empty()) {
            try {
                Stop stop = Stop::deserialize(line);
                // Проверяем на дубликаты перед добавлением
                bool exists = false;
                const auto& existingStops = system.getStops();
                for (const auto& existingStop : existingStops) {
                    if (existingStop.getId() == stop.getId()) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    system.addStopDirect(stop);
                    loadedCount++;
                }
            } catch (const std::exception& e) {
                throw FileException("stops.txt", "чтение строки " + std::to_string(lineNumber) + ": " + e.what());
            }
        }
    }
    if (file.bad() && !file.eof()) {
        throw FileException("stops.txt", "ошибка чтения файла");
    }
    file.close();
}

void DataManager::loadVehicles(TransportSystem& system) {
    std::ifstream file(dataDirectory + "vehicles.txt");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        if (!line.empty()) {
            try {
                std::istringstream ss(line);
                std::string type, model, licensePlate;
                std::getline(ss, type, '|');
                std::getline(ss, model, '|');
                std::getline(ss, licensePlate);

                // Проверяем на дубликаты перед добавлением
                bool exists = false;
                const auto& existingVehicles = system.getVehicles();
                for (const auto& existingVehicle : existingVehicles) {
                    if (existingVehicle->getLicensePlate() == licensePlate) {
                        exists = true;
                        break;
                    }
                }
                
                if (!exists) {
                    std::shared_ptr<Vehicle> vehicle;
                    if (type == "Автобус") {
                        vehicle = std::make_shared<Bus>(model, licensePlate);
                    } else if (type == "Трамвай") {
                        vehicle = std::make_shared<Tram>(model, licensePlate);
                    } else if (type == "Троллейбус") {
                        vehicle = std::make_shared<Trolleybus>(model, licensePlate);
                    } else {
                        throw InputException("Неизвестный тип транспорта: " + type);
                    }

                    system.addVehicleDirect(vehicle);
                    loadedCount++;
                }
            } catch (const std::exception& e) {
                throw FileException("vehicles.txt", "чтение строки " + std::to_string(lineNumber) + ": " + e.what());
            }
        }
    }
    if (file.bad() && !file.eof()) {
        throw FileException("vehicles.txt", "ошибка чтения файла");
    }
    file.close();
}

void DataManager::loadDrivers(TransportSystem& system) {
    std::ifstream file(dataDirectory + "drivers.txt");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        if (!line.empty()) {
            try {
                // Проверяем, что строка не содержит данные транспорта (формат: тип|модель|номер)
                // Водители должны быть в формате: имя|фамилия|отчество
                // Если в строке есть цифры в первой части или формат не соответствует, пропускаем
                std::istringstream testStream(line);
                std::string firstPart, secondPart, thirdPart;
                std::getline(testStream, firstPart, '|');
                std::getline(testStream, secondPart, '|');
                std::getline(testStream, thirdPart);
                
                // Проверяем, что первая часть не является типом транспорта
                if (firstPart == "Автобус" || firstPart == "Трамвай" || firstPart == "Троллейбус") {
                    continue;
                }
                
                // Проверяем, что первая часть выглядит как имя (не номерной знак транспорта)
                // Номерные знаки обычно содержат пробелы и цифры в определенном формате
                // Имена обычно начинаются с буквы и не содержат только цифры
                if (firstPart.length() > 0 && std::isdigit(static_cast<unsigned char>(firstPart[0]))) {
                    // Начинается с цифры - похоже на номерной знак или ID, пропускаем
                    continue;
                }
                
                auto driver = Driver::deserialize(line);
                // Проверяем на дубликаты перед добавлением
                bool exists = false;
                const auto& existingDrivers = system.getDrivers();
                for (const auto& existingDriver : existingDrivers) {
                    if (existingDriver->getFirstName() == driver->getFirstName() &&
                        existingDriver->getLastName() == driver->getLastName() &&
                        existingDriver->getMiddleName() == driver->getMiddleName()) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    system.addDriverDirect(driver);
                    loadedCount++;
                }
            } catch (const std::exception& e) {
                // Не бросаем исключение, просто пропускаем неправильную строку
                continue;
            }
        }
    }
    if (file.bad() && !file.eof()) {
        throw FileException("drivers.txt", "ошибка чтения файла");
    }
    file.close();
}

void DataManager::loadRoutes(TransportSystem& system) {
    std::ifstream file(dataDirectory + "routes.txt");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        if (!line.empty()) {
            try {
                auto route = Route::deserialize(line);
                // Проверяем на дубликаты перед добавлением
                bool exists = false;
                const auto& existingRoutes = system.getRoutes();
                for (const auto& existingRoute : existingRoutes) {
                    if (existingRoute->getNumber() == route->getNumber()) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    system.addRouteDirect(route);
                    loadedCount++;
                }
            } catch (const std::exception& e) {
                throw FileException("routes.txt", "чтение строки " + std::to_string(lineNumber) + ": " + e.what());
            }
        }
    }
    if (file.bad() && !file.eof()) {
        throw FileException("routes.txt", "ошибка чтения файла");
    }
    file.close();
}

void DataManager::loadTrips(TransportSystem& system) {
    std::ifstream file(dataDirectory + "trips.txt");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    int lineNumber = 0;
    int loadedCount = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        if (!line.empty()) {
            try {
                auto trip = Trip::deserialize(line, &system);
                // Проверяем на дубликаты перед добавлением
                bool exists = false;
                const auto& existingTrips = system.getTrips();
                for (const auto& existingTrip : existingTrips) {
                    if (existingTrip->getTripId() == trip->getTripId()) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    system.addTripDirect(trip);
                    loadedCount++;
                }
            } catch (const std::exception& e) {
                // Не бросаем исключение, просто пропускаем неправильную строку
                continue;
            }
        }
    }
    if (file.bad() && !file.eof()) {
        throw FileException("trips.txt", "ошибка чтения файла");
    }
    file.close();
}

void DataManager::loadAdminCredentials(TransportSystem& system) {
    std::ifstream file(dataDirectory + "admins.bin", std::ios::binary);
    if (!file.is_open()) return;

    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    std::unordered_map<std::string, std::string> creds;
    for (size_t i = 0; i < count; ++i) {
        size_t usernameLen, passwordLen;
        file.read(reinterpret_cast<char*>(&usernameLen), sizeof(usernameLen));
        std::string username(usernameLen, '\0');
        file.read(&username[0], usernameLen);

        file.read(reinterpret_cast<char*>(&passwordLen), sizeof(passwordLen));
        std::string password(passwordLen, '\0');
        file.read(&password[0], passwordLen);

        creds[username] = password;
    }
    file.close();

    system.setAdminCredentials(creds);
}

