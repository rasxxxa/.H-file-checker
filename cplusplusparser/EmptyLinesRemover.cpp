#include "EmptyLinesRemover.h"

[[maybe_unused]]std::vector<std::vector<std::string>> EmptyLinesRemover::ParseLines(std::vector<std::string>& lines)
{
    std::vector<std::string> lines_without_new_lines;
    for (const auto& line : lines)
    {
        if (!IsOnlySpaces(line))
            lines_without_new_lines.push_back(line);
    }

    lines = std::vector(lines_without_new_lines);
    return std::vector<std::vector<std::string>>();
}
