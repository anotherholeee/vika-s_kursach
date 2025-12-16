#include "driver.h"
#include <sstream>

Driver::Driver(const std::string& fname, const std::string& lname, const std::string& mname)
    : firstName(fname), lastName(lname), middleName(mname) {}

std::string Driver::getFullName() const {
    return lastName + " " + firstName + (middleName.empty() ? "" : " " + middleName);
}

std::string Driver::getFirstName() const {
    return firstName;
}

std::string Driver::getLastName() const {
    return lastName;
}

std::string Driver::getMiddleName() const {
    return middleName;
}

bool Driver::operator==(const Driver& other) const {
    return firstName == other.firstName &&
           lastName == other.lastName &&
           middleName == other.middleName;
}

std::string Driver::serialize() const {
    return firstName + "|" + lastName + "|" + middleName;
}

std::shared_ptr<Driver> Driver::deserialize(const std::string& data) {
    std::istringstream ss(data);
    std::string firstName, lastName, middleName;
    std::getline(ss, firstName, '|');
    std::getline(ss, lastName, '|');
    std::getline(ss, middleName);
    return std::make_shared<Driver>(firstName, lastName, middleName);
}

