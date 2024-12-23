#include "file.hpp"

#include <fstream>
#include <iostream>

namespace files
{

std::vector<std::string> readFile(const std::string& file_name) {
    std::vector<std::string> lines;
    std::ifstream file(file_name);
    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();

    return lines;
}

void writeFile(const std::string& file_name, const std::vector<std::string>& lines) {
    std::ofstream file(file_name);

    for (const auto& line : lines) {
        file << line << '\n';
    }

    file.close();
}

void replaceLine(std::vector<std::string>& lines, size_t line_number, const std::string new_line) {
    if (line_number >= 0 && line_number < lines.size()) {
        lines[line_number] = new_line;
    } else {
        std::cerr << "Invalid file line number\n";
    }
}

void deleteLine(std::vector<std::string>& lines, size_t line_number) {
    if (line_number >= 0 && line_number < lines.size()) {
        lines.erase(lines.begin() + static_cast<long>(line_number));
    } else {
        std::cerr << "Invalid file line number\n";
    }
}

void addLine(std::vector<std::string>& lines, const std::string new_line) {
    lines.push_back(new_line);
}
 
} // namespace files
