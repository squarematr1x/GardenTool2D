#include "profiler.hpp"

#include <thread>
#include <algorithm>

Profiler::Profiler() {
    writeHeader();
}

Profiler::~Profiler() {
    writeFooter();
    m_fout.close();
}

void Profiler::writeResult(const ProfileResult& result) {
    std::lock_guard<std::mutex> lock(m_lock);

    if (m_count++ > 0) {
        m_fout << ',';
    }

    std::string name = result.name;
    std::replace(name.begin(), name.end(), '"', '\'');

    m_fout << "\n{";
    m_fout << "\"cat\":\"function\",";
    m_fout << "\"dur\":" << (result.end - result.start) << ',';
    m_fout << "\"name\":\"" << name << "\",";
    m_fout << "\"ph\":\"X\",";
    m_fout << "\"pid\":0,";
    m_fout << "\"tid\":" << result.thread_id << ",";
    m_fout << "\"ts\":" << result.start;
    m_fout << "}";
}



ProfileTimer::ProfileTimer(const std::string& name) {
    m_result.name = name;
    start();
}

void ProfileTimer::start() {
    static long long last_start_t = 0;

    m_stp = std::chrono::high_resolution_clock::now();
    m_result.start = std::chrono::time_point_cast<std::chrono::microseconds>(m_stp).time_since_epoch().count();

    if (m_result.start == last_start_t) {
        // To mitigate visual issues with chrome:/tracing
        m_result.start++;
    }

    last_start_t = m_result.start;
    m_stopped = false;


}

void ProfileTimer::stop() {
    if (m_stopped) {
        return;
    }

    auto etp = std::chrono::high_resolution_clock::now();
    m_result.end = std::chrono::time_point_cast<std::chrono::microseconds>(etp).time_since_epoch().count();
    m_result.thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());

    Profiler::Instance().writeResult(m_result);
    m_stopped = true;
}
