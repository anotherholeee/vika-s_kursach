#include "stop.h"
#include <sstream>

Stop::Stop(int stopId, std::string stopName) : id(stopId), name(std::move(stopName)) {}

int Stop::getId() const {
    return id;
}

std::string Stop::getName() const {
    return name;
}

bool Stop::operator==(const Stop& other) const {
    return id == other.id;
}

std::string Stop::serialize() const {
    return std::to_string(id) + "|" + name;
}

Stop Stop::deserialize(const std::string& data) {
    std::istringstream ss(data);
    std::string idStr, name;
    std::getline(ss, idStr, '|');
    std::getline(ss, name);
    return Stop(std::stoi(idStr), name);
}

