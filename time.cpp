#include "time.h"
#include <sstream>

void Time::normalize(int totalMinutes) {
    totalMinutes = ((totalMinutes % 1440) + 1440) % 1440;
    hours = totalMinutes / 60;
    minutes = totalMinutes % 60;
}

Time::Time(int h, int m) {
    normalize(h * 60 + m);
}

Time::Time(const std::string& timeStr) {
    std::stringstream ss(timeStr);
    char colon;
    int h, m;
    if (!(ss >> h >> colon >> m) || colon != ':' || ss.fail() || !ss.eof()) {
        throw InputException("Неверный формат времени: " + timeStr);
    }
    if (h < 0 || h > 23 || m < 0 || m > 59) {
        throw InputException("Некорректное время: " + timeStr);
    }
    hours = h;
    minutes = m;
}

int Time::getTotalMinutes() const {
    return hours * 60 + minutes;
}

int Time::getHours() const {
    return hours;
}

int Time::getMinutes() const {
    return minutes;
}

bool Time::operator<(const Time& other) const {
    return getTotalMinutes() < other.getTotalMinutes();
}

bool Time::operator<=(const Time& other) const {
    return getTotalMinutes() <= other.getTotalMinutes();
}

bool Time::operator>(const Time& other) const {
    return getTotalMinutes() > other.getTotalMinutes();
}

bool Time::operator>=(const Time& other) const {
    return getTotalMinutes() >= other.getTotalMinutes();
}

bool Time::operator==(const Time& other) const {
    return getTotalMinutes() == other.getTotalMinutes();
}

bool Time::operator!=(const Time& other) const {
    return getTotalMinutes() != other.getTotalMinutes();
}

Time Time::operator+(int minutesToAdd) const {
    return Time(0, getTotalMinutes() + minutesToAdd);
}

Time Time::operator-(int minutesToSubtract) const {
    return Time(0, getTotalMinutes() - minutesToSubtract);
}

int Time::operator-(const Time& other) const {
    return getTotalMinutes() - other.getTotalMinutes();
}

std::ostream& operator<<(std::ostream& os, const Time& time) {
    os << (time.hours < 10 ? "0" : "") << time.hours << ":"
       << (time.minutes < 10 ? "0" : "") << time.minutes;
    return os;
}

std::istream& operator>>(std::istream& is, Time& time) {
    std::string timeStr;
    is >> timeStr;
    time = Time(timeStr);
    return is;
}

std::string Time::serialize() const {
    return (hours < 10 ? "0" : "") + std::to_string(hours) + ":" +
           (minutes < 10 ? "0" : "") + std::to_string(minutes);
}

Time Time::deserialize(const std::string& data) {
    return Time(data);
}

