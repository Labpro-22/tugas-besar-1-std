#include "../../include/data/TransactionLogger.hpp"

#include <sstream>

string TransactionLogger::formatEntry(int turn, const string& username,const string& actionType,const string& detail) const {
    ostringstream oss;
    oss << "[Turn " << turn << "] " << username
        << " | " << actionType << " | " << detail;
    return oss.str();
}

void TransactionLogger::log(int turn, const string& username, const string& actionType, const string& detail) {
    logs.push_back(formatEntry(turn, username, actionType, detail));
}

vector<string> TransactionLogger::getAll() const {
    return logs;
}

vector<string> TransactionLogger::getRecent(int count) const {
    vector<string> out;
    if (count <= 0) return out;
    int total = static_cast<int>(logs.size());
    if (count >= total) return logs;
    for (int i = total - count; i < total; ++i) {
        out.push_back(logs[i]);
    }
    return out;
}

int TransactionLogger::getCount() const {
    return static_cast<int>(logs.size());
}

void TransactionLogger::clear() {
    logs.clear();
}
