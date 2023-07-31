#include "DefineRemover.h"

std::vector<std::vector<std::string>> DefineRemover::ParseLines(std::vector<std::string>& lines)
{
    // Remove ifdefs from lines and return them
// This case return 1 dimension vector lines
    std::vector<std::vector<std::string>> ifdef_lines;
    std::vector<std::string> lines_without_ifdef;
    std::vector<std::string> ifdefs;
    for (const auto& line : lines)
    {
        if (!line.contains("#"))
            lines_without_ifdef.push_back(line);
        else
            ifdefs.push_back(line);
    }

    lines = std::vector(lines_without_ifdef);
    ifdef_lines.push_back(ifdefs);
    return ifdef_lines;
}
