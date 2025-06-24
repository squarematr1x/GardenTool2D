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

    return lines;
}

void writeFile(const std::string& file_name, const std::vector<std::string>& lines) {
    std::ofstream file(file_name);

    if (!file) {
        std::cerr << "Error: Could not open file " << file_name << '\n';
        return;
    }

    for (const auto& line : lines) {
        file << line << '\n';
    }
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

void createLevel(const std::string& lvl_name, LevelType lvl_type) {
    std::string lvl_type_str = "sc.lvl";
    std::vector<std::string> content;

    switch (lvl_type) {
        case LevelType::SIDE_SCROLLER: {
            lvl_type_str = "sc.lvl";
            content = {
                "# Player: x, y, bboxw, bboxh, speed, max speed, jump speed, gravity, weapon animation name hp",
                "Player 64 2 64 64 5 5 25 0 Fire 4"
            };
            break;
        }
        case LevelType::TOPDOWN_RPG: {
            lvl_type_str = "rpg.lvl";
            content = {
                "# Player: x, y, bboxw, bboxh, speed, max speed, jump speed, gravity, weapon animation name hp",
                "Player 9 6 58 58 5 0 0 0 Empty 4"
            };
            break;
        }
        default: break;
    }
    const auto path = "config/levels/" + lvl_name + "." + lvl_type_str;
    writeFile(path, content);
}

} // namespace files
