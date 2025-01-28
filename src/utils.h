#pragma once

#include <chrono>

#ifdef DEBUG
#define LOG(msg) printf(msg);
#define LOGF(msg, ...) printf(msg, __VA_ARGS__);
#else
#define LOG(msg)
#define LOGF(msg, ...)
#endif


typedef std::chrono::high_resolution_clock Clock;

class Timer {
public:
    Timer() {
        restart();
    }
    void restart() {
        m_lastTime = Clock::now();
    }
    double ms() {
        auto currTime = Clock::now();
        return (std::chrono::duration<double, std::milli>(currTime - m_lastTime)).count();
    }
    ~Timer() = default;
private:
    Clock::time_point m_lastTime;
};