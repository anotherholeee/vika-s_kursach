#ifndef TIME_H
#define TIME_H

#include <string>
#include <iostream>
#include "exceptions.h"

class Time {
private:
    int hours;
    int minutes;

    void normalize(int totalMinutes);

public:
    explicit Time(int h = 0, int m = 0);
    explicit Time(const std::string& timeStr);

    int getTotalMinutes() const;
    int getHours() const;
    int getMinutes() const;

    bool operator<(const Time& other) const;
    bool operator<=(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator>=(const Time& other) const;
    bool operator==(const Time& other) const;
    bool operator!=(const Time& other) const;

    Time operator+(int minutesToAdd) const;
    Time operator-(int minutesToSubtract) const;
    int operator-(const Time& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Time& time);
    friend std::istream& operator>>(std::istream& is, Time& time);

    std::string serialize() const;
    static Time deserialize(const std::string& data);
};

#endif // TIME_H

