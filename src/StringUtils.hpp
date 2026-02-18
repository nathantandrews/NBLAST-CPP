#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>

int basename(const std::string& str, std::string& res);
int basenameNoExt(const std::string& str, std::string& res);
int splitOnComma(const std::string& str, std::pair<std::string, std::string>& res);
int stringToUInt(const std::string& str, uint64_t& res);

#endif // STRING_UTILS_HPP