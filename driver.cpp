#include "driver.h"
#include <sstream>

Driver::Driver(const std::string& fname, const std::string& lname, const std::string& mname, const std::string& cat)
    : firstName(fname), lastName(lname), middleName(mname), category(cat) {}

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

std::string Driver::getCategory() const {
    return category;
}

void Driver::setCategory(const std::string& cat) {
    category = cat;
}

bool Driver::operator==(const Driver& other) const {
    return firstName == other.firstName &&
           lastName == other.lastName &&
           middleName == other.middleName;
}

std::string Driver::serialize() const {
    return firstName + "|" + lastName + "|" + middleName + "|" + category;
}

std::shared_ptr<Driver> Driver::deserialize(const std::string& data) {
    std::istringstream ss(data);
    std::string firstName, lastName, middleName, category;
    std::getline(ss, firstName, '|');
    std::getline(ss, lastName, '|');
    std::getline(ss, middleName, '|');
    std::getline(ss, category);
    // Если категория не указана в старых данных, оставляем пустой строкой
    return std::make_shared<Driver>(firstName, lastName, middleName, category);
}

