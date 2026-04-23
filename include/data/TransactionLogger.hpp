#ifndef TRANSACTION_LOGGER_HPP
#define TRANSACTION_LOGGER_HPP

#include <string>
#include <vector>

using namespace std;


class TransactionLogger {
private:
    vector<string> logs;

    string formatEntry(int turn, const string& username,
                            const string& actionType,
                            const string& detail) const;

public:
    void log(int turn, const string& username,
             const string& actionType, const string& detail);
    vector<string> getAll() const;
    vector<string> getRecent(int count) const;
    int getCount() const;
    void clear();
};

#endif
