#include "CommentRemover.h"

namespace
{
    bool IsCommentLine(const std::string& line, std::string& formatted)
    {
        auto pos = line.find("//");
        if (pos == 0)
            return true;
        else if (pos != std::string::npos && pos <= 2)
        {
            return true;
        }
        else if (pos != std::string::npos && pos > 2)
        {
            formatted = line.substr(0, pos);
            return true;
        }

        static bool insideMultiLineComment = false;
        if (!insideMultiLineComment) 
        {
            if (line.find("/*") == 0) 
            {
                insideMultiLineComment = true;
                return true;
            }
        }
        else 
        {
            if (line.find("*/") != std::string::npos) 
            {
                insideMultiLineComment = false;
            }
            return true;
        }
        return false;
    }
}

std::vector<std::string> CommentRemover::ParseLines(std::vector<std::string>& lines)
{
	// Remove comments from lines and return them
    std::vector<std::string> comments_lines, lines_without_comments;

    for (const auto& line : lines)
    {
        std::string formatted;
        if (!IsCommentLine(line, formatted))
            lines_without_comments.push_back(line);
        else if (!formatted.empty())
            lines_without_comments.push_back(formatted);
        else
            comments_lines.push_back(line);
    }

    lines = std::vector(lines_without_comments);
    return comments_lines;
}
