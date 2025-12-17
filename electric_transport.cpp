#include "electric_transport.h"

ElectricTransport::ElectricTransport(const std::string& t, const std::string& m, const std::string& lp, 
                      int cap, double volt)
    : Vehicle(t, m, lp), capacity(cap), voltage(volt) {}

int ElectricTransport::getCapacity() const {
    return capacity;
}

void ElectricTransport::setCapacity(int cap) {
    capacity = cap;
}

double ElectricTransport::getVoltage() const {
    return voltage;
}

void ElectricTransport::setVoltage(double volt) {
    voltage = volt;
}

std::string ElectricTransport::serialize() const {
    return Vehicle::serialize() + "|" + std::to_string(capacity) + "|" + 
           std::to_string(voltage);
}

