#include "../../include/utils/FileWriteException.hpp"

FileWriteException::FileWriteException(const std::string& msg)
    : message(msg) {}

const char* FileWriteException::what() const {
    return message.c_str();
}

const std::string& FileWriteException::getMessage() const {
    return message;
}
