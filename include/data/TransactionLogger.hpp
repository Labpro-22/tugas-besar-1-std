#ifndef TRANSACTION_LOGGER_HPP
#define TRANSACTION_LOGGER_HPP

#include <string>
#include <vector>

using namespace std;


class TransactionLogger {
private:
    std::vector<std::string> logs;

    std::string formatEntry(int turn, const std::string& username,
                            const std::string& actionType,
                            const std::string& detail) const;

public:
    void log(int turn, const std::string& username,
             const std::string& actionType, const std::string& detail);
    std::vector<std::string> getAll() const;
    std::vector<std::string> getRecent(int count) const;
    int getCount() const;
    void clear();
};

#endif
