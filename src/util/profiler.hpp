#pragma once

#include <fstream>
#include <mutex>
#include <chrono>

#define PROFILING 1

#ifdef PROFILING
    #define PROFILE_SCOPE(name) \
        ProfileTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION() \
        PROFILE_SCOPE(__FUNCTION__)
#else
    #define PROFILE_SCOPE(name)
    #define PROFILE_FUNCTION()
#endif

struct ProfileResult {
    std::string name{ "Default" };
    long long start{ 0 };
    long long end{ 0 };
    size_t thread_id{ 0 };
};

class Profiler {
    std::string m_output_file{ "profresult.json" };
    std::ofstream m_fout = std::ofstream(m_output_file);
    size_t m_count{ 0 };
    std::mutex m_lock;

    Profiler();

public:
    static Profiler& Instance() {
        static Profiler instance;
        return instance;
    }

    void writeHeader() { m_fout << "{\"otherData\": {},\"traceEvents\":["; }
    void writeFooter() { m_fout << "]}"; }

    void writeResult(const ProfileResult& res);

    ~Profiler();
};

class ProfileTimer {
    ProfileResult m_result;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_stp; // starting point
    bool m_stopped{ false };

    void start();
    void stop();

public:
    ProfileTimer(const std::string& name);
    ~ProfileTimer() { stop(); }
};
