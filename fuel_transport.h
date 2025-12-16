#ifndef FUEL_TRANSPORT_H
#define FUEL_TRANSPORT_H

#include "vehicle.h"

// Промежуточный класс для топливного транспорта (Уровень 2)
class FuelTransport : public Vehicle {
protected:
    int capacity; // Вместимость пассажиров
    std::string fuelType; // Тип топлива (бензин, дизель, газ)
public:
    FuelTransport(const std::string& t, const std::string& m, const std::string& lp, 
                  int cap = 50, const std::string& fuel = "дизель");

    int getCapacity() const;
    void setCapacity(int cap);
    std::string getFuelType() const;
    void setFuelType(const std::string& fuel);

    std::string getInfo() const override;
    std::string serialize() const override;
};

#endif // FUEL_TRANSPORT_H

