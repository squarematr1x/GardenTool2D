#pragma once

#include <chrono>

class Timer
{
    long long m_start{ 0 };
    long long m_end{ 0 };

    std::chrono::time_point<std::chrono::high_resolution_clock> m_stp; // starting point

public:
    Timer() { start(); }

    void start() { m_stp = std::chrono::high_resolution_clock::now(); }

    long long elapsed();
};
