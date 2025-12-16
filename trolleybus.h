#ifndef TROLLEYBUS_H
#define TROLLEYBUS_H

#include "electric_transport.h"

// Класс троллейбуса (Уровень 3)
class Trolleybus : public ElectricTransport {
public:
    Trolleybus(const std::string& m, const std::string& lp, int cap = 50, double volt = 600.0);
};

#endif // TROLLEYBUS_H

