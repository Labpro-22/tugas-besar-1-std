#ifndef FILE_FORMAT_EXCEPTION_HPP
#define FILE_FORMAT_EXCEPTION_HPP

#include <string>

class FileFormatException {
private:
    std::string message;

public:
    explicit FileFormatException(const std::string& msg);
    const char* what() const;
    const std::string& getMessage() const;
};

#endif
