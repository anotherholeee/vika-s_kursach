#include "exceptions.h"

TransportException::TransportException(const std::string& msg) : message(msg) {}

const char* TransportException::what() const noexcept {
    return message.c_str();
}

InputException::InputException(const std::string& msg) : TransportException(msg) {}

FileException::FileException(const std::string& filename, const std::string& operation)
    : TransportException("Ошибка работы с файлом '" + filename + "' (" + operation + ")") {}

FileException::FileException(const std::string& msg) : TransportException("Ошибка файла: " + msg) {}

ContainerException::ContainerException(const std::string& msg) : TransportException("Ошибка контейнера: " + msg) {}

