#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <stack>
#include <regex>
#include <unordered_map>
#include <filesystem>

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
    return line.contains("public:") || line.contains("private:") || line.contains("protected:") || line.contains("#");
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


std::vector<std::string> ParseVariable(const std::string& line)
{
    std::string variable;
    if (!line.contains(";"))
        return std::vector<std::string>();

    std::vector<std::string> variables;

    auto pos = line.find(";") - 1;
    int posLeft = 0;
    int posRight = -1;
    bool usable = true;

    do
    {
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
                while (pos >= 0 && line[pos] != '{')
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
            return std::vector<std::string>();
        }

        auto IsAcceptableSign = [](char a)
        {
            return std::isalpha(a) || std::isdigit(a) || a == '_';
        };

        while (pos >= 0 && IsAcceptableSign(line[pos]))
            pos--;

        variables.push_back(line.substr(pos + 1, posRight - pos));
        std::string restOfLine = line.substr(0, pos);

        if (restOfLine.back() - 1 == ',' || restOfLine.back() - 2 == ',' || restOfLine.back() == ',')
            pos = restOfLine.find_last_of(',') - 1;
        else
            usable = false;

    } while (usable);

    return variables;
}

void RemoveEnums(std::vector < std::string>& lines)
{
    size_t pos = 0;
    std::vector<std::string> withoutEnums;
    while (pos < lines.size())
    {
        if (!lines[pos].contains("enum"))
        {
            withoutEnums.push_back(lines[pos]);
        }
        else
        {
            while (pos < lines.size() && !lines[pos].contains("};"))
                pos++;
        }
        pos++;
    }
    lines = std::vector<std::string>(withoutEnums);
}

std::set<std::string> GetUniqueVariables(std::vector<std::string>& lines)
{
    std::set<std::string> unique_variables;
    std::set<std::string> methods;
    std::set<std::string> structs;
    std::vector<std::string> linesWithoutComments;
    // delete all comments 
    RemoveEnums(lines);
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
        auto vars = ParseVariable(copy);
        if (vars.size())
        {
            for (auto valS : vars)
                unique_variables.insert(valS);
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
#ifdef _DEBUG
    std::string filesPath = "D:\\magicnhd\\BiCa53G";
    std::string s = "";
    if (argc > 1)
        s = argv[1];
#else
    std::string filesPath = ".";
    std::string s = argv[1];
#endif 
    
    if (s.contains("REC"))
    {
        struct Files
        {
            std::string hFile;
            std::string cppFile;
        };

        std::vector<Files> toCheck;

        for (auto path : std::filesystem::directory_iterator(filesPath.c_str()))
        {
            if (!std::filesystem::is_directory(path))
            {
                std::string fileName = path.path().string();
                if (fileName.back() == 'h' && fileName[fileName.size() - 2] == '.')
                {
                    Files f;
                    f.hFile = fileName;
                    f.cppFile = fileName.substr(0, fileName.size() - 2);
                    f.cppFile.append(".cpp");
                    toCheck.push_back(f);
                }
            }
        }

        for (const auto& val : toCheck)
        {
            std::cout << std::endl << std::endl << std::endl;
            std::cout << "------FILE------" << std::endl;
            std::cout << val.hFile << std::endl << std::endl << std::endl;

            auto linesH = ReadFile(val.hFile);
            auto linesCpp = ReadFile(val.cppFile);

            auto values = GetUniqueVariables(linesH);
            ClearCppFileFromComments(linesCpp);
            auto occurence = GetOccurenceOfVariables(values, linesCpp);
            for (const auto& val : occurence)
            {
                std::cout << "Variable " << val.first << " : " << val.second << " times!" << std::endl;
            }
        }
    }
    else
    {
        std::string hFile, cppFile;
#ifdef _DEBUG
        hFile = "DBingoCard.h";
        cppFile = "DBingoCard.cpp";
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
    }


    return 0;
}
