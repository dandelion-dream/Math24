//
// Created by alfred on 2026/11/26.
//

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::stringstream;

vector<string> string_split_char(const string& s, char c) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, c)) {
        result.push_back(item);
    }
    return result;
}

vector<string> string_split_str(const string& s, const string& separator) {
    std::vector<std::string> result;
    size_t start = 0, end = 0;
    while ((end = s.find(separator, start)) != std::string::npos) {
        result.push_back(s.substr(start, end - start));
        start = end + separator.length();
    }
    result.push_back(s.substr(start));
    return result;
}