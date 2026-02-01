#ifndef LOGGING_HPP
#define LOGGING_HPP

#include "FileIO.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <ctime>

enum class LogLevel {
    debug = 0,
    info,
    warn,
    error
};

struct LoggerConfig {
    LogLevel level = LogLevel::info;
    bool showTimestamps = true;
    std::string filepath;
};

inline LoggerConfig& getLoggerConfig() {
    static LoggerConfig config;
    return config;
}

inline std::ofstream& getLogFile() {
    static std::ofstream file;
    return file;
}

inline const char* levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::debug: return "DEBUG";
        case LogLevel::info:  return "INFO";
        case LogLevel::warn:  return "WARN";
        case LogLevel::error: return "ERROR";
        default:              return "UNKNOWN";
    }
}

inline std::string makeTimestampedFilename(
    const std::string& prefix = "run",
    const std::string& ext = ".log"
) {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
    localtime_r(&t, &tm);

    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", &tm);

    return prefix + "-" + buf + ext;
}

inline void openLogFile(const std::string& prefix = "log/run") {
    auto& f = getLogFile();
    if (f.is_open()) f.close();

    std::string filename = makeTimestampedFilename(prefix);

    ensureDirectory(filename);

    f.open(filename, std::ios::out | std::ios::trunc);
    if (!f) {
        std::cerr << "Failed to open log file: " << filename << "\n";
    }

    getLoggerConfig().filepath = filename;
}


inline void logPrintf(LogLevel lvl, const char* fmt, ...) {
    if (lvl < getLoggerConfig().level) return;

    // format message
    char msgbuf[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);
    va_end(args);

    // timestamp
    char timebuf[32] = "";
    if (getLoggerConfig().showTimestamps) {
        std::time_t t = std::time(nullptr);
        std::tm tm{};
        localtime_r(&t, &tm);
        std::strftime(timebuf, sizeof(timebuf), "%F %T", &tm);
    }

    auto write = [&](std::ostream& os) {
        if (getLoggerConfig().showTimestamps) {
            os << timebuf << " ";
        }
        os << "[" << levelToString(lvl) << "] "
           << msgbuf << "\n";
        os.flush();
    };

    write(std::cerr);

    auto& f = getLogFile();
    if (f.is_open()) {
        write(f);
    }
}

#define LOG_DEBUG(fmt, ...) logPrintf(LogLevel::debug, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  logPrintf(LogLevel::info,  fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  logPrintf(LogLevel::warn,  fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) logPrintf(LogLevel::error, fmt, ##__VA_ARGS__)

#ifdef NDEBUG
#undef LOG_DEBUG
#define LOG_DEBUG(...) ((void)0)
#endif

#endif // LOGGING_HPP
