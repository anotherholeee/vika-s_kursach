#include "vehicle.h"
#include <sstream>

Vehicle::Vehicle(const std::string& t, const std::string& m, const std::string& lp)
    : type(t), model(m), licensePlate(lp) {}

std::string Vehicle::getInfo() const {
    return type + " " + model + " " + licensePlate;
}

std::string Vehicle::getType() const {
    return type;
}

std::string Vehicle::getModel() const {
    return model;
}

std::string Vehicle::getLicensePlate() const {
    return licensePlate;
}

std::string Vehicle::serialize() const {
    return type + "|" + model + "|" + licensePlate;
}

std::shared_ptr<Vehicle> Vehicle::deserialize(const std::string& data) {
    std::istringstream ss(data);
    std::string type, model, licensePlate;
    std::getline(ss, type, '|');
    std::getline(ss, model, '|');
    std::getline(ss, licensePlate);

    throw ContainerException("Используйте DataManager для создания транспортных средств");
}

