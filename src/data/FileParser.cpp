#include "../../include/data/FileParser.hpp"

#include <sstream>

vector<string> FileParser::tokenizeLine(const string& line) const {
    vector<string> tokens;
    istringstream iss(line);
    string tok;
    while (iss >> tok) {
        tokens.push_back(tok);
    }
    return tokens;
}

string FileParser::trim(const string& s) const {
    size_t begin = 0;
    while (begin < s.size() &&
           (s[begin] == ' ' || s[begin] == '\t' ||
            s[begin] == '\r' || s[begin] == '\n')) {
        ++begin;
    }
    size_t end = s.size();
    while (end > begin &&
           (s[end - 1] == ' ' || s[end - 1] == '\t' ||
            s[end - 1] == '\r' || s[end - 1] == '\n')) {
        --end;
    }
    return s.substr(begin, end - begin);
}
