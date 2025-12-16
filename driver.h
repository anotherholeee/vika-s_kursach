#ifndef DRIVER_H
#define DRIVER_H

#include <string>
#include <memory>
#include <sstream>

class Driver {
private:
    std::string firstName;
    std::string lastName;
    std::string middleName;
public:
    Driver(const std::string& fname, const std::string& lname, const std::string& mname = "");

    std::string getFullName() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getMiddleName() const;

    bool operator==(const Driver& other) const;

    std::string serialize() const;
    static std::shared_ptr<Driver> deserialize(const std::string& data);
};

#endif // DRIVER_H

