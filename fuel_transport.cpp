#include "fuel_transport.h"

FuelTransport::FuelTransport(const std::string& t, const std::string& m, const std::string& lp, 
                  int cap, const std::string& fuel)
    : Vehicle(t, m, lp), capacity(cap), fuelType(fuel) {}

int FuelTransport::getCapacity() const {
    return capacity;
}

void FuelTransport::setCapacity(int cap) {
    capacity = cap;
}

std::string FuelTransport::getFuelType() const {
    return fuelType;
}

void FuelTransport::setFuelType(const std::string& fuel) {
    fuelType = fuel;
}

std::string FuelTransport::serialize() const {
    return Vehicle::serialize() + "|" + std::to_string(capacity) + "|" + fuelType;
}

