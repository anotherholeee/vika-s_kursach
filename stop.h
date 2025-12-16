#ifndef STOP_H
#define STOP_H

#include <string>

class Stop {
private:
    int id;
    std::string name;
public:
    Stop(int stopId, std::string stopName);

    int getId() const;
    std::string getName() const;

    bool operator==(const Stop& other) const;

    std::string serialize() const;
    static Stop deserialize(const std::string& data);
};

#endif // STOP_H

