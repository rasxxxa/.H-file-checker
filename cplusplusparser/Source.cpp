#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <stack>
#include <regex>
#include <unordered_map>
#include <filesystem>

struct ClassParser
{
    std::set<std::string> unique_variables;
    std::set<std::string> private_methods;
    std::set<std::string> public_methods;
    std::set<std::string> static_methods;
    std::string class_name;
    std::string global_pointer;
};

enum class Visibility
{
    Private,
    Public
};

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
            bool nameFound = false;
            if (lines[pos].contains("{"))
            {
                parentesis.push('{');
            }

            auto posOfStruct = lines[pos].find("struct");
            posOfStruct += 7;

            std::string structName = "";
            while (posOfStruct < lines[pos].size() && lines[pos][posOfStruct] != '{')
            {
                if (lines[pos][posOfStruct] != ' ')
                    structName.push_back(lines[pos][posOfStruct]);
                posOfStruct++;
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
            if (!structName.empty())
            {
                structs.insert(structName);
            }
            else
            {
                std::string copyLine = lines[pos];
                auto it = std::remove_if(copyLine.begin(), copyLine.end(), [](char a) {

                    return a == ' ' || a == '}' || a == '{' || a == ';';

                    });

                copyLine.erase(it, copyLine.end());

                structs.insert(copyLine);
            }


            
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


std::vector<std::string> ParseVariable(const std::string& line, ClassParser& parser)
{
    std::string variable;
    if (!line.contains(";"))
        return std::vector<std::string>();

    std::vector<std::string> variables;
    bool isExtern = line.find("extern") != std::string::npos;
    int pos = static_cast<int>(line.find(";") - 1);
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
        if (isExtern)
            parser.global_pointer = variables.back();
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

void RemoveNamespaces(std::vector < std::string>& lines)
{
    size_t pos = 0;
    std::vector<std::string> withoutEnums;
    while (pos < lines.size())
    {
        if (!lines[pos].contains("namespace"))
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


void RemoveMethods(std::vector<std::string>& lines, std::vector<std::string>& returnMethods, ClassParser& parser)
{
    size_t pos = 0;
    Visibility defaultVis = Visibility::Private;
    std::vector<std::string> withoudMethods;
    while (pos < lines.size())
    {
        if (lines[pos].contains("private:") || lines[pos].contains("protected:"))
            defaultVis = Visibility::Private;
        else if (lines[pos].contains("public:"))
            defaultVis = Visibility::Public;


        if (!lines[pos].contains("("))
        {
            withoudMethods.push_back(lines[pos]);
        }
        else
        {
            bool isStatic = false;
            if (lines[pos].contains("static"))
            {
                isStatic = true;
            }
            auto posOfParentesis = lines[pos].find("(") -1;
            std::string Method = "";
            while (posOfParentesis >= 0 && lines[pos][posOfParentesis] != ' ')
            {
                Method.push_back(lines[pos][posOfParentesis]);
                posOfParentesis--;
            }
            std::reverse(Method.begin(), Method.end());
            returnMethods.push_back(Method);

            const static auto FilterMethod = [](const std::string& line_to_check)
            {
                return !line_to_check.contains("operator") && !line_to_check.contains("=") && !line_to_check.contains("&&") && !line_to_check.contains("~");
            };

            if (FilterMethod(Method))
            {
                const auto it = std::remove_if(Method.begin(), Method.end(), [](char a)
                {
                        return a == '&' || a == ' ' || a == '*';
                });
                Method.erase(it, Method.end());
                if (defaultVis == Visibility::Private)
                    parser.private_methods.insert(Method);
                else
                    parser.public_methods.insert(Method);

                if (isStatic)
                    parser.static_methods.insert(Method);
            }
            
            while (pos < lines.size() && (!lines[pos].contains(")")))
                pos++;
        }
        pos++;
    }
    lines = std::vector<std::string>(withoudMethods);
};



ClassParser GetUniqueVariables(std::vector<std::string>& lines)
{
    ClassParser parser;

    std::set<std::string> unique_variables;
    std::set<std::string> methods;
    std::set<std::string> structs;
    std::vector<std::string> linesWithoutComments;

    for (const auto& line : lines)
    {
        std::string formatted;
        if (!IsCommentLine(line, formatted))
        {
            linesWithoutComments.push_back(line);
        }

        if (!formatted.empty())
            linesWithoutComments.push_back(formatted);
    }

    std::vector<std::string> linesWithoutNewLines;

    for (const auto& line : linesWithoutComments)
    {
        if (IsNewLineOrEmpty(line))
            continue;

        if (IsClass(line))
        {
            if (parser.class_name.empty())
            {
                size_t pos = line.find_first_not_of("class");
                if (pos != std::string::npos)
                {
                    while (pos != line.size() && (!std::isalpha(line[pos])) || line[pos] == '_') pos++;
                    std::string className;
                    while (std::isalpha(line[pos]) || line[pos] == '_') { className.push_back(line[pos]); pos++; }
                    if (!className.empty()) parser.class_name = className;
                }
            }
            continue;
        }
        linesWithoutNewLines.push_back(line);
    }


    RemoveEnums(linesWithoutNewLines);

	structs = FindAllStructs(linesWithoutNewLines);

    std::vector<std::string> methodNames;
    RemoveMethods(linesWithoutNewLines, methodNames, parser);
    RemoveNamespaces(linesWithoutNewLines);

    for (const auto& val : linesWithoutNewLines)
    {
        std::string copy = val;
        auto vars = ParseVariable(copy, parser);
        if (!vars.empty())
        {
            for (auto valS : vars)
                unique_variables.insert(valS);
        }
    }
    parser.unique_variables = unique_variables;
    if (!parser.global_pointer.empty())
    {
        parser.unique_variables.erase(parser.global_pointer);
    }
    return parser;
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


std::unordered_map<std::string, int> GetOccurenceOfVariables(const ClassParser& parser, const std::vector<std::string>& lines)
{
    std::unordered_map<std::string, int> occurences;

    for (const auto& val : parser.unique_variables)
        occurences[val] = 0;

    for (const auto& val : parser.unique_variables)
    {
        for (const auto& line : lines)
        {
            if (line.contains(val))
                occurences[val]++;
        }
    }

    return occurences;
}


std::unordered_map<std::string, int> GetMethodOcurence(const ClassParser& parser, const std::vector<std::vector<std::string>>& files)
{
    std::unordered_map<std::string, int> ocurences;
    struct MethodPossible
    {
        std::string key_name;
        std::vector<std::string> possible_names;
    };
    std::vector<MethodPossible> all_combination;

    for (const auto& line : parser.public_methods)
    {
        ocurences[line] = 0;
        MethodPossible method_possible;
        method_possible.key_name = line;
        if (!parser.global_pointer.empty())
        {
            std::string fullName = parser.global_pointer;
            fullName.append("->");
            fullName.append(line);
            method_possible.possible_names.push_back(fullName);
        }
        else if (!parser.class_name.empty())
        {
            std::string fullName = parser.class_name;
            fullName.append("::");
            fullName.append(line);
            method_possible.possible_names.push_back(fullName);
        }
        if (!method_possible.possible_names.empty())
            all_combination.push_back(method_possible);
    }

    for (const auto& line : parser.static_methods)
    {
        ocurences[line] = 0;
        MethodPossible method_possible;
        method_possible.key_name = line;
        if (!parser.global_pointer.empty())
        {
            std::string fullName = parser.global_pointer;
            fullName.append("->");
            fullName.append(line);
            method_possible.possible_names.push_back(fullName);
        }

        if (!parser.class_name.empty())
        {
            std::string fullName = parser.class_name;
            fullName.append("::");
            fullName.append(line);
            method_possible.possible_names.push_back(fullName);
        }

        if (!method_possible.possible_names.empty())
            all_combination.push_back(method_possible);
    }

    for (const auto& possible_combination : all_combination)
    {
	    for (const auto& file : files)
	    {
		    for (const auto& lineInFile : file)
		    {
			    for (const auto& combination : possible_combination.possible_names)
			    {
				   if (lineInFile.contains(combination))
				   {
                       ocurences[possible_combination.key_name]++;
				   }
			    }
		    }
	    }
    }

    return ocurences;
}


#define WRITE_TO_FILE

int main(int argc, const char** argv) 
{
#ifdef _DEBUG
    std::string filesPath = "D:\\magicnhd\\BiCa61A";
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

        std::unordered_map<std::string, ClassParser> mapped_parsers;

        std::vector<Files> toCheck;

        for (auto path : std::filesystem::recursive_directory_iterator(filesPath.c_str()))
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
                    if (std::filesystem::exists(f.cppFile))
                        toCheck.push_back(f);
                }
            }
        }
        std::ofstream file("UnusedVariables.txt");
        std::unordered_map <std::string, std::vector < std::string >> allFiles;
        for (const auto& val : toCheck)
        {
            
#ifdef WRITE_TO_FILE
            file << std::endl << std::endl << std::endl;
            file << "------FILE------" << std::endl;
            file << val.hFile << std::endl << std::endl << std::endl;
#else 
            std::cout << std::endl << std::endl << std::endl;
            std::cout << "------FILE------" << std::endl;
            std::cout << val.hFile << std::endl << std::endl << std::endl;
#endif
            auto linesH = ReadFile(val.hFile);
            auto linesCpp = ReadFile(val.cppFile);

            auto parser = GetUniqueVariables(linesH);
            ClearCppFileFromComments(linesCpp);
            allFiles[val.hFile] = (linesCpp);
            mapped_parsers[val.hFile] = parser;
            auto occurence = GetOccurenceOfVariables(parser, linesCpp);

            struct mapval
            {
                std::string s;
                int occ;
            };

            std::vector<mapval> valuesM;

            for (const auto& val : occurence)
            {
                mapval m;
                m.s = val.first;
                m.occ = val.second;
                if (m.occ <= 5)
                    valuesM.push_back(m);
            }

            std::sort(valuesM.begin(), valuesM.end(), [](const mapval& first, const mapval& second) {
                
                return first.occ < second.occ;
                
                });

            for (const auto& mVal : valuesM)
            {
#ifdef WRITE_TO_FILE
                file << "Variable " << mVal.s << " - ocurence : " << mVal.occ << " times!" << std::endl;
#else
                std::cout << "Variable " << mVal.s << " - ocurence : " << mVal.occ << " times!" << std::endl;
#endif
            }


        }

        for (const auto& check_all_parsers : mapped_parsers)
        {
            std::vector<std::vector<std::string>> all_files;
            for (const auto& lines : allFiles)
            {
                all_files.push_back(lines.second);
            }

            auto usage = GetMethodOcurence(check_all_parsers.second, all_files);

            struct mapvalS
            {
                std::string s;
                int occ;
            };
            std::vector<mapvalS> sortedMethods;
            for (const auto& v : usage)
            {
                mapvalS m;
                m.s = v.first;
                m.occ = v.second;
                if (m.occ <= 5)
                    sortedMethods.push_back(m);
            }

            std::sort(sortedMethods.begin(), sortedMethods.end(), [](const mapvalS& first, const mapvalS& second) {

                return first.occ < second.occ;

                });

            file << std::endl << std::endl << std::endl << "Class : " << check_all_parsers.first << std::endl;
            for (const auto& methods : sortedMethods)
            {
                file << "Method " << methods.s << " - ocurence : " << methods.occ<< " times!" << std::endl;
            }

        }

        file.flush();
        file.close();
    }
    else
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
        std::ofstream file("UnusedVariables.txt");
        for (const auto& val : occurence)
        {
#ifdef WRITE_TO_FILE
            file << "Variable " << val.first << " : " << val.second << " times!" << std::endl;
#else
            std::cout << "Variable " << val.first << " : " << val.second << " times!" << std::endl;
#endif
        }
        file.flush();
        file.close();
    }


    return 0;
}
