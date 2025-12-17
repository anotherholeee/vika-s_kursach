#ifndef ELECTRIC_TRANSPORT_H
#define ELECTRIC_TRANSPORT_H

#include "vehicle.h"

// Промежуточный класс для электрического транспорта (Уровень 2)
class ElectricTransport : public Vehicle {
protected:
    int capacity; // Вместимость пассажиров
    double voltage; // Напряжение питания (В)
public:
    ElectricTransport(const std::string& t, const std::string& m, const std::string& lp, 
                      int cap = 50, double volt = 600.0);

    int getCapacity() const;
    void setCapacity(int cap);
    double getVoltage() const;
    void setVoltage(double volt);

    std::string serialize() const override;
};

#endif // ELECTRIC_TRANSPORT_H

