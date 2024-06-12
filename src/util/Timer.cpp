#include "timer.hpp"

long long Timer::elapsed() {
    auto etp = std::chrono::high_resolution_clock::now();
    m_start = std::chrono::time_point_cast<std::chrono::microseconds>(m_stp).time_since_epoch().count();
    m_end = std::chrono::time_point_cast<std::chrono::microseconds>(etp).time_since_epoch().count();

    return m_end - m_start;
}
