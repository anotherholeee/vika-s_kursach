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
    std::string category; // Категория водительских прав (D, T, и т.д.)
public:
    Driver(const std::string& fname, const std::string& lname, const std::string& mname = "", const std::string& cat = "");

    std::string getFullName() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getMiddleName() const;
    std::string getCategory() const;
    void setCategory(const std::string& cat);

    bool operator==(const Driver& other) const;

    std::string serialize() const;
    static std::shared_ptr<Driver> deserialize(const std::string& data);
};

#endif // DRIVER_H

