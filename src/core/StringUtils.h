#pragma once

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace StringUtils
{

inline std::string Trim(const std::string& s)
{
    const char* whitespace = " \t\r\n";
    const auto first = s.find_first_not_of(whitespace);
    if (first == std::string::npos) return {};
    const auto last = s.find_last_not_of(whitespace);
    return s.substr(first, last - first + 1);
}

inline std::string ToLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) 
        { 
            return static_cast<char>(std::tolower(c)); 
        });
    return s;
}

inline bool EqualsIgnoreCase(const std::string& a, const std::string& b)
{
    return ToLower(a) == ToLower(b);
}

inline std::vector<std::string> SplitWhitespace(const std::string& s)
{
    std::istringstream in(s);
    std::vector<std::string> tokens;
    std::string token;
    while (in >> token) tokens.push_back(token);
    return tokens;
}

inline std::vector<std::string> SplitLines(const std::string& s)
{
    std::istringstream in(s);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) lines.push_back(line);
    return lines;
}

}
