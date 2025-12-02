#include "Utils.hpp"

#include <string>
#include <vector>
#include <fstream>

std::string basename(const std::string& filePath) {
    std::size_t pos = filePath.find_last_of("/\\");
    if (pos == std::string::npos) {
        return filePath;
    }
    return filePath.substr(pos + 1);
}

std::string basenameNoExt(const std::string& filePath) {
    std::string base = basename(filePath);
    std::size_t pos = base.find_last_of('.');
    if (pos != std::string::npos) {
        base = base.substr(0, pos);
    }
    return base;
}

unsigned computeLineCount(std::ifstream& fin) {
    fin.seekg(0, std::ios::beg);
    const size_t bufferSize = 1 << 20;
    std::vector<char> buffer(bufferSize);
    unsigned lineCount = 0;
    while (fin) {
        fin.read(buffer.data(), buffer.size());
        std::streamsize bytes_read = fin.gcount();
        for (std::streamsize i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n') ++lineCount;
        }
    }
    fin.clear();
    fin.seekg(0, std::ios::beg);
    return lineCount + 1; // in case no comments, no newline at end.
}
