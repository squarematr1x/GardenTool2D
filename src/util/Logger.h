#pragma once

#include <fstream>

// A singleton class for logging
class Logger 
{
    std::ofstream m_fout{ std::ofstream("log.txt") };

public:
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    void log(const std::string& str);
};
