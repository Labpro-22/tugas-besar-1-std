#include "../../include/utils/FileFormatException.hpp"

FileFormatException::FileFormatException(const std::string& msg)
    : message(msg) {}

const char* FileFormatException::what() const {
    return message.c_str();
}

const std::string& FileFormatException::getMessage() const {
    return message;
}
