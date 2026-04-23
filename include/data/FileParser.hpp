#ifndef FILE_PARSER_HPP
#define FILE_PARSER_HPP

#include <string>
#include <vector>

using namespace std;


class FileParser {
protected:
    vector<string> tokenizeLine(const string& line) const;
    string trim(const string& s) const;
};

#endif
