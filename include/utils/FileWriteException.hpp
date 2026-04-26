#ifndef FILE_WRITE_EXCEPTION_HPP
#define FILE_WRITE_EXCEPTION_HPP

#include <string>

class FileWriteException {
private:
    std::string message;

public:
    explicit FileWriteException(const std::string& msg);
    const char* what() const;
    const std::string& getMessage() const;
};

#endif
