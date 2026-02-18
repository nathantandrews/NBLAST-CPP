#include "StringUtils.hpp"

#include <string>
#include <stdexcept>

int basename(const std::string& str, std::string& res) {
    std::size_t pos = str.find_last_of("/\\");
    if (pos == std::string::npos) {
        res = str;
        return -1;
    }
    res = str.substr(pos + 1);
    return 0;
}
int basenameNoExt(const std::string& str, std::string& res) {
    int rc = basename(str, res);
    if (rc) {
        return rc;
    }
    std::size_t pos = res.find_last_of('.');
    if (pos == std::string::npos) {
        return -1;
    }
    res = res.substr(0, pos);
    return 0;
}
int splitOnComma(const std::string& str, std::pair<std::string, std::string>& res) {
    auto pos = str.find(',');
    if (pos == std::string::npos) {
        res.first = str;
        res.second = "";
        return -1;
    }
    res.first = str.substr(0, pos);
    res.second = str.substr(pos+1);
    return 0;
}
int stringToUInt(const std::string& str, uint64_t& res) {
    try {
        res = std::stoul(str);
        return 0;
    } catch (const std::invalid_argument& e) {
        return -1;
    } catch (const std::out_of_range& e) {
        return -2;
    }
}
