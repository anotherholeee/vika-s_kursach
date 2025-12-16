#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

// Базовый класс для исключений транспортной системы
class TransportException : public std::exception {
protected:
    std::string message;
public:
    explicit TransportException(const std::string& msg);
    const char* what() const noexcept override;
    virtual ~TransportException() = default;
};

// Исключение для ошибок ввода данных
class InputException : public TransportException {
public:
    explicit InputException(const std::string& msg);
};

// Исключение для ошибок работы с файлами
class FileException : public TransportException {
public:
    explicit FileException(const std::string& filename, const std::string& operation = "операция");
    explicit FileException(const std::string& msg);
};

// Исключение для ошибок работы с контейнерами
class ContainerException : public TransportException {
public:
    explicit ContainerException(const std::string& msg);
};

#endif // EXCEPTIONS_H

