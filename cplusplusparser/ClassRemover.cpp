#include "ClassRemover.h"

namespace
{
    inline bool IsClass(const std::string& line)
    {
        std::regex pattern("\\bclass\\b");
        return std::regex_search(line, pattern);
    }
}

std::vector<std::vector<std::string>> ClassRemover::ParseLines(std::vector<std::string>& lines)
{
    std::vector<std::vector<std::string>> classes;
    std::vector<std::vector<std::string>> classes_temp;
    std::vector<std::string> other_lines;
    size_t pos = 0;
    bool any_class_found = false;
    size_t classes_number = 0;
    size_t opened_parenthesis = 0;
    size_t classes_found_number = 0;
    while (pos < lines.size())
    {
        if (IsClass(lines[pos]))
        {
            std::vector<std::string> class_found;
            class_found.push_back(lines[pos]);
            classes_temp.push_back(class_found);
            any_class_found = true;
            classes_number++;
            classes_found_number++;
            Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
            opened_parenthesis += parenthesis_number.left;
            opened_parenthesis -= parenthesis_number.right;
            if (opened_parenthesis < classes_number && parenthesis_number.left > 0)
            {
                classes.push_back(classes_temp.back());
                classes_temp.pop_back();

                if (!--classes_number)
                    any_class_found = false;
            }
        }
        else if (any_class_found)
        {
            classes_temp[classes_temp.size() - 1].push_back(lines[pos]);
            Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
            opened_parenthesis += parenthesis_number.left;
            opened_parenthesis -= parenthesis_number.right;
            if (opened_parenthesis < classes_number)
            {
                classes.push_back(classes_temp.back());
                classes_temp.pop_back();

                if (!--classes_number)
                    any_class_found = false;
            }
        }
        else if (lines[pos].find("extern") != std::string::npos)
        {
            classes[classes.size() - 1].push_back(lines[pos]);
        }
        else
        {
            other_lines.push_back(lines[pos]);
        }
        pos++;
    }

    lines = std::vector(other_lines);
    return classes;
}
