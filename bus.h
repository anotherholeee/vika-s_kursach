#ifndef BUS_H
#define BUS_H

#include "fuel_transport.h"

// Класс автобуса (Уровень 3)
class Bus : public FuelTransport {
public:
    Bus(const std::string& m, const std::string& lp, int cap = 50, const std::string& fuel = "дизель");
};

#endif // BUS_H

