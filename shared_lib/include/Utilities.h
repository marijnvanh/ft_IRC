#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <iostream>

#include <string>
#include <vector>

/**
 * @brief Splits the given string on the given delimiters, including empty matches.
 * 
 * @param str The string to split using the given delimiters.
 * @param delimiters A string containing the delimiters to split on.
 */
inline std::vector<std::string> split(const std::string& str, const std::string& delimiters)
{
    size_t prev = 0, pos = 0;
    std::vector<std::string> tokens;

    do
    {
        pos = str.find_first_of(delimiters, prev);
        if (pos == std::string::npos)
			pos = str.length();

        std::string token = str.substr(prev, pos - prev);
		tokens.push_back(token);

        prev = pos + 1;
    }
    while (pos < str.length() && prev < str.length());

    return (tokens);
}

auto GetErrorMessage(int error, std::string param = "") -> std::string;

#endif
