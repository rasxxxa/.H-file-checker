#include "StructRemover.h"

namespace
{
    inline bool IsStruct(const std::string& line)
    {
        std::regex pattern("\\bstruct\\b");
        return std::regex_search(line, pattern);
    }
}

std::vector<std::vector<std::string>> StructRemover::ParseLines(std::vector<std::string>& lines)
{
    std::vector<std::vector<std::string>> structs;
    std::vector<std::vector<std::string>> structs_temp;
    std::vector<std::string> other_lines;
    size_t pos = 0;
    bool any_struct_found = false;
    size_t structs_number = 0;
    size_t opened_parenthesis = 0;
    size_t structs_found_number = 0;
    while (pos < lines.size())
    {
        if (IsStruct(lines[pos]))
        {
            std::vector<std::string> struct_found;
            struct_found.push_back(lines[pos]);
            structs_temp.push_back(struct_found);
            any_struct_found = true;
            structs_number++;
            structs_found_number++;
            Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
            opened_parenthesis += parenthesis_number.left;
            opened_parenthesis -= parenthesis_number.right;
            if (opened_parenthesis < structs_number && parenthesis_number.left > 0)
            {
                structs.push_back(structs_temp.back());
                structs_temp.pop_back();

                if (!--structs_number)
                    any_struct_found = false;
            }
        }
        else if (any_struct_found)
        {
            structs_temp[structs_temp.size() - 1].push_back(lines[pos]);
            Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
            opened_parenthesis += parenthesis_number.left;
            opened_parenthesis -= parenthesis_number.right;
            if (opened_parenthesis < structs_number)
            {
                structs.push_back(structs_temp.back());
                structs_temp.pop_back();

                if (!--structs_number)
                    any_struct_found = false;
            }
        }
        else
        {
            other_lines.push_back(lines[pos]);
        }
        pos++;
    }

    lines = std::vector(other_lines);
    return structs;
}
