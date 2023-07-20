#include "NamespaceRemover.h"

namespace
{
    inline bool IsNamespace(const std::string& line)
    {
        std::regex pattern("\\bnamespace\\b");
        return std::regex_search(line, pattern);
    }
}

std::vector<std::vector<std::string>> NamespaceRemover::ParseLines(std::vector<std::string>& lines)
{
    std::vector<std::vector<std::string>> namespaces;
    std::vector<std::vector<std::string>> namespaces_temp;
    std::vector<std::string> other_lines;
    size_t pos = 0;
    bool any_namespace_found = false;
    size_t namespaces_number = 0;
    size_t opened_parenthesis = 0;
    size_t namespaces_found_number = 0;
    while (pos < lines.size())
    {
        if (IsNamespace(lines[pos]))
        {
            std::vector<std::string> namespace_found;
            namespace_found.push_back(lines[pos]);
            namespaces_temp.push_back(namespace_found);
            any_namespace_found = true;
            namespaces_number++;
            namespaces_found_number++;
            Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
            opened_parenthesis += parenthesis_number.left;
            opened_parenthesis -= parenthesis_number.right;
            if (opened_parenthesis < namespaces_number && parenthesis_number.left > 0)
            {
                namespaces.push_back(namespaces_temp.back());
                namespaces_temp.pop_back();

                if (!--namespaces_number)
                    any_namespace_found = false;
            }
        }
        else if (any_namespace_found)
        {
            namespaces_temp[namespaces_temp.size() - 1].push_back(lines[pos]);
            Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
            opened_parenthesis += parenthesis_number.left;
            opened_parenthesis -= parenthesis_number.right;
            if (opened_parenthesis < namespaces_number)
            {
                namespaces.push_back(namespaces_temp.back());
                namespaces_temp.pop_back();

                if (!--namespaces_number)
                    any_namespace_found = false;
            }
        }
        else
        {
            other_lines.push_back(lines[pos]);
        }
        pos++;
    }

    lines = std::vector(other_lines);
    return namespaces;
}
