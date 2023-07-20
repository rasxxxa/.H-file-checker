#include "EnumRemover.h"

namespace
{
    inline bool IsEnum(const std::string& line)
    {
        std::regex pattern("\\benum\\b");
        return std::regex_search(line, pattern);
    }
}

std::vector<std::vector<std::string>> EnumRemover::ParseLines(std::vector<std::string>& lines)
{
    std::vector<std::vector<std::string>> enums;
    std::vector<std::vector<std::string>> enums_temp;
    std::vector<std::string> other_lines;
    size_t pos = 0;
    bool any_enum_found = false;
    size_t enums_number = 0;
    size_t opened_parenthesis = 0;
    size_t enumss_found_number = 0;
    while (pos < lines.size())
    {
        if (IsEnum(lines[pos]))
        {
            std::vector<std::string> enum_found;
            enum_found.push_back(lines[pos]);
            enums_temp.push_back(enum_found);
            any_enum_found = true;
            enums_number++;
            enumss_found_number++;
            Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
            opened_parenthesis += parenthesis_number.left;
            opened_parenthesis -= parenthesis_number.right;
            if (opened_parenthesis < enums_number && parenthesis_number.left > 0)
            {
                enums.push_back(enums_temp.back());
                enums_temp.pop_back();

                if (!--enums_number)
                    any_enum_found = false;
            }
        }
        else if (any_enum_found)
        {
            enums_temp[enums_temp.size() - 1].push_back(lines[pos]);
            Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
            opened_parenthesis += parenthesis_number.left;
            opened_parenthesis -= parenthesis_number.right;
            if (opened_parenthesis < enums_number)
            {
                enums.push_back(enums_temp.back());
                enums_temp.pop_back();

                if (!--enums_number)
                    any_enum_found = false;
            }
        }
        else
        {
            other_lines.push_back(lines[pos]);
        }
        pos++;
    }

    lines = std::vector(other_lines);
    return enums;
}
