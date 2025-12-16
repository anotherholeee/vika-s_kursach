#ifndef TRAM_H
#define TRAM_H

#include "electric_transport.h"

// Класс трамвая (Уровень 3)
class Tram : public ElectricTransport {
public:
    Tram(const std::string& m, const std::string& lp, int cap = 100, double volt = 600.0);
};

#endif // TRAM_H

