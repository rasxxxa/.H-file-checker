#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <stack>
#include <regex>
#include <unordered_map>

std::vector<std::string> ReadFile(const std::string_view& filePath)
{
    std::vector<std::string> file;
    std::ifstream fileopened(filePath.data());
    if (fileopened.is_open())
    {
        std::string line;
        while (std::getline(fileopened, line))
        {
            file.push_back(line);
        }

        fileopened.close();
    }

    return file;
};



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


    // Check for multi-line comment
    // (Assuming no nested multi-line comments)
    static bool insideMultiLineComment = false;
    if (!insideMultiLineComment) {
        if (line.find("/*") == 0) {
            insideMultiLineComment = true;
            return true;
        }
    }
    else {
        if (line.find("*/") != std::string::npos) {
            insideMultiLineComment = false;
        }
        return true;
    }



    return false;
}



bool IsNewLineOrEmpty(const std::string& line)
{
    return line.size() <= 2;
}


bool IsMethod(const std::string& line)
{
    if (line.contains("(") && line.contains(")"))
        return true;

    return false;
}


bool IsPrivateProtectedOrPublic(const std::string& line)
{
    return line.contains("public:") || line.contains("private:") || line.contains("protected:") || line.contains("pragma");
}

bool IsEnum(const std::string& line)
{
    if (line.contains("enum"))
        return true;
    return false;
}

std::set<std::string> FindAllStructs(std::vector<std::string>& lines)
{
    std::vector<std::string> newLines;
    size_t pos = 0;
    std::set<std::string> structs;
    while (pos != lines.size())
    {
        std::regex pattern("\\bstruct\\b");
        if (std::regex_search(lines[pos], pattern))
        {
            std::stack<char> parentesis;
            if (lines[pos].contains("{"))
            {
                parentesis.push('{');
            }

            while (parentesis.empty() && pos + 1 != lines.size())
            {
                pos++;
                if (lines[pos].contains("{"))
                    parentesis.push('{');
            }

            while (!parentesis.empty() && pos + 1 != lines.size())
            {
                pos++;
                if (lines[pos].contains("}"))
                {
                    parentesis.pop();
                }
                else if (lines[pos].contains("{"))
                {
                    parentesis.push('{');
                }
            }
            std::string copyLine = lines[pos];
            auto it = std::remove_if(copyLine.begin(), copyLine.end(), [](char a) {

                return a == ' ' || a == '}' || a == '{' || a == ';';

                });

            copyLine.erase(it, copyLine.end());

            structs.insert(copyLine);

            
        }
        else
        {
            newLines.push_back(lines[pos]);
        }
        pos++;
    }
    lines = std::vector(newLines);
    return structs;
}


bool IsClass(const std::string& line)
{
    std::regex pattern("\\bclass\\b");
    if (std::regex_search(line, pattern))
    {
        return true;
    }

    return false;
}


bool ParseVariable(std::string& line)
{
    std::string variable;
    if (!line.contains(";"))
        return false;

    auto pos = line.find(";") - 1;
    int posLeft = 0;
    int posRight = -1;
    while (pos >= 0)
    {
        if (line[pos] == ']')
        {
            while (pos >= 0 && line[pos] != '[')
                pos--;
            pos--;
        }
        else if (line[pos] == '}')
        {
            while (pos >= 0 && line[pos] != '[')
                pos--;
            pos--;
        }
        else
        {
            posRight = pos;
            break;
        }
    }

    if (posRight == -1)
    {
        return false;
    }

    auto IsAcceptableSign = [](char a)
    {
        return std::isalpha(a) || std::isdigit(a) || a == '_';
    };

    while (pos>= 0 && IsAcceptableSign(line[pos]))
        pos--;

    line = line.substr(pos + 1, posRight - pos);
 

    return true;
}


std::set<std::string> GetUniqueVariables(const std::vector<std::string>& lines)
{
    std::set<std::string> unique_variables;
    std::set<std::string> methods;
    std::set<std::string> structs;
    std::vector<std::string> linesWithoutComments;
    // delete all comments 
    for (const auto& line : lines)
    {
        if (IsNewLineOrEmpty(line))
            continue;

        if (IsPrivateProtectedOrPublic(line))
            continue;

        if (IsMethod(line))
        {
            methods.insert(line);
            continue;
        }

        if (IsClass(line))
            continue;


        if (IsEnum(line))
        {
            // Handle enums
            continue;
        }


        std::string formatted;
        if (!IsCommentLine(line, formatted))
        {
            linesWithoutComments.push_back(line);
        }

        if (!formatted.empty())
            linesWithoutComments.push_back(formatted);
    }


    structs = FindAllStructs(linesWithoutComments);

    for (const auto& val : linesWithoutComments)
    {
        std::string copy = val;
        if (ParseVariable(copy))
        {
            unique_variables.insert(copy);
        }
    }


    return unique_variables;
}


void ClearCppFileFromComments(std::vector<std::string>& lines)
{
    std::vector<std::string> copy;
    for (auto& val : lines)
    {
        std::string formatted;
        if (!IsCommentLine(val, formatted))
        {
            copy.push_back(val);
        }

        if (!formatted.empty())
            copy.push_back(formatted);
    }

    lines = std::vector<std::string>(copy);
}


std::unordered_map<std::string, int> GetOccurenceOfVariables(const std::set<std::string>& variables, const std::vector<std::string>& lines)
{
    std::unordered_map<std::string, int> occurences;

    for (const auto& val : variables)
        occurences[val] = 0;

    for (const auto& val : variables)
    {
        for (const auto& line : lines)
        {
            if (line.contains(val))
                occurences[val]++;
        }
    }

    return occurences;
}


int main(int argc, const char** argv) 
{
    std::string hFile, cppFile;
#ifdef _DEBUG
    hFile = "DBingoCards.h";
    cppFile = "DBingoCards.cpp";
#else
    hFile = argv[1];
    cppFile = argv[2];
#endif

    auto linesH = ReadFile(hFile);
    auto linesCpp = ReadFile(cppFile);

    auto values = GetUniqueVariables(linesH);
    ClearCppFileFromComments(linesCpp);
    auto occurence = GetOccurenceOfVariables(values, linesCpp);
    for (const auto& val : occurence)
    {
        std::cout << "Variable " << val.first << " : " << val.second << " times!" << std::endl;
    }


    return 0;
}
