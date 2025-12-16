#include "tram.h"

Tram::Tram(const std::string& m, const std::string& lp, int cap, double volt)
    : ElectricTransport("Трамвай", m, lp, cap, volt) {}

