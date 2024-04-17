#include "Logger.hpp"

void Logger::log(const std::string& str) {
    m_fout << str << '\n';
    m_fout.close();
}
