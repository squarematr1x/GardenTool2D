#pragma once

#include <vector>

namespace files
{

std::vector<std::string> readFile(const std::string& file_name);

void writeFile(const std::string& file_name, const std::vector<std::string>& lines);

void replaceLine(std::vector<std::string>& lines, size_t line_number, const std::string new_line);
void deleteLine(std::vector<std::string>& lines, size_t line_number);
void addLine(std::vector<std::string>& lines, const std::string new_line);

} // namespace files
