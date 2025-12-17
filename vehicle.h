#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <memory>
#include "exceptions.h"

// Базовый класс для транспортного средства (Уровень 1)
class Vehicle {
protected:
    std::string type;
    std::string model;
    std::string licensePlate;
public:
    Vehicle(const std::string& t, const std::string& m, const std::string& lp);

    virtual ~Vehicle() = default;

    std::string getInfo() const;
    std::string getType() const;
    std::string getModel() const;
    std::string getLicensePlate() const;

    virtual std::string serialize() const;
    static std::shared_ptr<Vehicle> deserialize(const std::string& data);
};

#endif // VEHICLE_H

