#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <typeinfo>
#include "../LevelEditor/gameEntity.h"

enum class LogLevel
{
    Info,
    Warning,
    Error
};

struct LogEntry
{
    std::string message;
    LogLevel level;
};

inline std::vector<LogEntry> logBuffer;
// Log buffer so it doesn't get too big, Value not yet determined
constexpr size_t MAX_LOG_ENTRIES = 1000;

inline void PushLogEntry(const LogEntry &entry)
{
    if (logBuffer.size() >= MAX_LOG_ENTRIES)
        logBuffer.erase(logBuffer.begin());
    logBuffer.push_back(entry);
}

// Converting primitives to strings
inline std::string ToString(const std::string &s) { return s; }
inline std::string ToString(const char *s) { return s ? std::string(s) : "[null]"; }
inline std::string ToString(char c) { return std::string(1, c); }
inline std::string ToString(int v) { return std::to_string(v); }
inline std::string ToString(float v) { return std::to_string(v); }
inline std::string ToString(double v) { return std::to_string(v); }
inline std::string ToString(bool v) { return v ? "true" : "false"; }

// Fallback for pointers (prints type name)
template <typename T>
inline std::string ToString(const T *obj)
{
    if (!obj)
        return "[null]";
    return std::string("[") + typeid(T).name() + "]";
}

// Specialization Templates
class GameEntity;
template <>
inline std::string ToString<GameEntity>(const GameEntity *obj)
{
    if (!obj)
        return "[null]";
    return obj->GetName();
}

// Logging Functions
template <typename... Args>
inline void DebugPrint(Args &&...args)
{
    std::ostringstream oss;
    ((oss << (oss.tellp() > 0 ? " " : "") << ToString(std::forward<Args>(args))), ...);
    PushLogEntry({oss.str(), LogLevel::Info});
}

template <typename... Args>
inline void DebugWarn(Args &&...args)
{
    std::ostringstream oss;
    ((oss << (oss.tellp() > 0 ? " " : "") << ToString(std::forward<Args>(args))), ...);
    PushLogEntry({oss.str(), LogLevel::Warning});
}

template <typename... Args>
inline void DebugError(Args &&...args)
{
    std::ostringstream oss;
    ((oss << (oss.tellp() > 0 ? " " : "") << ToString(std::forward<Args>(args))), ...);
    PushLogEntry({oss.str(), LogLevel::Error});
}
