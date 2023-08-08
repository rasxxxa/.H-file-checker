#include "Analyzer.h"

Analyzer::Analyzer(const std::string& path)
{
    m_path = path;
}

void Analyzer::Analize()
{
    if (!std::filesystem::is_directory(m_path))
    {
        //assert(false);
        return;
    }

    m_mapped_parsers.clear();
    m_files_to_check.clear();
    m_allFilesCPP.clear();
    m_usage_per_file_variables.clear();
    m_occurence_per_file_variables.clear();
    m_usage_per_file_methods.clear();

    for (auto path : std::filesystem::recursive_directory_iterator(m_path.c_str()))
    {
        if (!std::filesystem::is_directory(path))
        {
            std::string fileName = path.path().string();
            if (fileName.back() == 'h' && fileName[fileName.size() - 2] == '.')
            {
                Files file;
                file.h_File = fileName;
                file.cpp_File = fileName.substr(0, fileName.size() - 2);
                file.cpp_File.append(".cpp");
                if (std::filesystem::exists(file.cpp_File))
                    m_files_to_check.push_back(file);
            }
        }
    }

    for (const auto& file : m_files_to_check)
    {
        auto linesH = File::ReadFile(file.h_File);
        auto linesCpp = File::ReadFile(file.cpp_File);

        auto extracted = m_hParser.ExtractFromFile(linesH);
        linesCpp = m_cppParser.RefineFile(linesCpp);
        linesH = m_cppParser.RefineFile(linesH);
        m_allFilesCPP[file.h_File] = linesCpp;
        m_allFilesH[file.h_File] = linesH;
        m_mapped_parsers[file.h_File] = extracted;
    }

    for (const auto& check_all_parsers : m_mapped_parsers)
    {
        if (check_all_parsers.second.size())
        {
            for (const auto& parser : check_all_parsers.second)
            {
                auto occurrenceVarsReadWrite = CheckVariablesForUsage(parser.unique_variables, m_allFilesCPP[check_all_parsers.first]);
                for (const auto& variable : occurrenceVarsReadWrite)
                    m_usage_per_file_variables[check_all_parsers.first][parser.class_name][variable.first] = variable.second;

                auto occurrence = GetOccurenceOfVariables(parser, m_allFilesCPP[check_all_parsers.first]);
                for (const auto& variable : occurrence)
                    m_occurence_per_file_variables[check_all_parsers.first][parser.class_name][variable.first] = variable.second;

                if (!parser.global_pointer.empty())
                {
                    auto usageMethods = GetMethodOcurence(parser, m_allFilesCPP, check_all_parsers.first);
                    for (const auto& method : usageMethods)
                        m_usage_per_file_methods[check_all_parsers.first][parser.class_name][method.first] = method.second;
                }
                else
                {
                    auto usageMethods = GetMethodOcurenceWithoutGlobalPointer(parser, m_allFilesCPP, check_all_parsers.first);
                    for (const auto& method : usageMethods)
                        m_usage_per_file_methods[check_all_parsers.first][parser.class_name][method.first] = method.second;
                }
            }
        }
    }
}

void Analyzer::PrintResults(const std::string& path)
{
    std::vector<std::string> lines_to_print;
    std::vector<std::string> unique_classes;
    for (const auto& _class : m_mapped_parsers)
        unique_classes.push_back(_class.first);
    for (const auto& file : unique_classes)
    {
        std::stringstream file_stream;
        file_stream << "File: " << file << std::endl;

        if (m_usage_per_file_variables.contains(file))
        {
            for (const auto& _class : m_usage_per_file_variables.at(file))
            {
                std::vector<VariableUsage> variables_usages;
                for (const auto& variable : _class.second)
                {
                    VariableUsage usage{};
                    usage.usage = m_occurence_per_file_variables[file][_class.first][variable.first];
                    usage.variable = variable.first;
                    variables_usages.push_back(usage);
                }

                std::ranges::sort(variables_usages, [](const auto& usage1, const auto& usage2) {

                    return usage1.usage < usage2.usage;

                    });

                file_stream << "\tClass: " << std::endl;
                file_stream << "\t" << _class.first << std::endl << std::endl;
                file_stream << "\tVariable usage: " << std::endl;
                for (const auto& variable : variables_usages)
                {
                    file_stream << "\t\tVariable: " << variable.variable << " occurrence: " << variable.usage << std::endl;
                    file_stream << "\t\tRead times: " << m_usage_per_file_variables[file][_class.first][variable.variable].read_times << std::endl;
                    file_stream << "\t\tWrite times: " << m_usage_per_file_variables[file][_class.first][variable.variable].write_times << std::endl;
                    file_stream << "\t\tCalling method times: " << m_usage_per_file_variables[file][_class.first][variable.variable].method_times << std::endl;
                }
                file_stream << std::endl << std::endl;
            }
        }

        if (m_usage_per_file_methods.contains(file))
        {
            for (const auto& _class : m_usage_per_file_methods[file])
            {
                std::vector<VariableUsage> method_usage;

                for (const auto& method : m_usage_per_file_methods.at(file).at(_class.first))
                {
                    VariableUsage usage{};
                    usage.usage = method.second;
                    usage.variable = method.first;
                    method_usage.push_back(usage);
                }

                std::ranges::sort(method_usage, [](const auto& usage1, const auto& usage2) {

                    return usage1.usage < usage2.usage;

                    });

                file_stream << "\tMethods usage: " << std::endl;

                for (const auto& method : method_usage)
                {
                    file_stream << "\t\tMethod: " << method.variable << ", occurrence: " << method.usage << std::endl;
                }
                file_stream << std::endl << std::endl;
            }
        
        }
        lines_to_print.push_back(file_stream.str());
    }

    File::WriteToFile(path, lines_to_print);

}

std::unordered_map<std::string, int> Analyzer::GetOccurenceOfVariables(const MethodsVariableCounter& parser, const std::vector<std::string>& lines)
{
    std::unordered_map<std::string, int> occurences;

    for (const auto& variable : parser.unique_variables)
        occurences[variable] = 0;

    for (const auto& variable : parser.unique_variables)
    {
        for (const auto& line : lines)
        {
            //std::string regex = "\\b";
            //regex.append(line);
            //regex.append("\\b");
            //std::regex pattern(regex.c_str());
            //if (std::regex_search(line, pattern)) 
            //    occurences[variable]++;
            if (line.contains(variable))
                occurences[variable]++;
        }
    }

    return occurences;
}

std::unordered_map<std::string, VariableOccurrenceReadWrite> Analyzer::CheckVariablesForUsage(const std::set<std::string>& variables, const std::vector<std::string>& sourceFile)
{
    std::unordered_map<std::string, VariableOccurrenceReadWrite> variableOccurrenceReadWrite;

    for (const auto& variable : variables)
    {
        VariableOccurrenceReadWrite occurrenceReadWrite{};
        int num_of_writes = 0;
        int num_of_reads = 0;
        int num_of_function_calls = 0;
        for (const auto& line : sourceFile)
        {
            if (line.contains(variable))
            {
                auto varPos = line.find(variable);

                if (line.find('=') < varPos 
                    || (line.find('(') < varPos && line.find(')', varPos) != std::string::npos) 
                    || line.find("return") != std::string::npos 
                    || (line.find('[') < varPos && line.find(']', varPos) > varPos) 
                    || line.find("==") != std::string::npos 
                    || line.find("!=") != std::string::npos)
                    num_of_reads++;
                else if (line.find('=', varPos) != std::string::npos 
                    || line.find("++", varPos) != std::string::npos 
                    || line.find("--", varPos) != std::string::npos)
                    num_of_writes++;
                else if (line.find("->", varPos) != std::string::npos 
                    || line.find(variable + '.', varPos) != std::string::npos)
                    num_of_function_calls++;

            }
        }
        occurrenceReadWrite.write_times = num_of_writes;
        occurrenceReadWrite.read_times = num_of_reads;
        occurrenceReadWrite.method_times = num_of_function_calls;
        variableOccurrenceReadWrite[variable] = occurrenceReadWrite;
    }
    return variableOccurrenceReadWrite;
}

std::unordered_map<std::string, int> Analyzer::GetMethodOcurence(const MethodsVariableCounter& parser, const std::unordered_map<std::string, std::vector<std::string>>& files, const std::string& classForParser)
{
    std::unordered_map<std::string, int> ocurences;
    struct MethodPossible
    {
        std::string key_name;
        std::vector<std::string> possible_names;
    };
    std::vector<MethodPossible> all_combination;
    std::vector<MethodPossible> only_main_cpp;

    for (const auto& line : parser.public_methods)
    {
        if (line == parser.class_name)
            continue;
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

        MethodPossible mp;
        mp.key_name = line;
        mp.possible_names.push_back(line);
        only_main_cpp.push_back(mp);

        if (!method_possible.possible_names.empty())
            all_combination.push_back(method_possible);
    }

    for (const auto& line : parser.private_methods)
    {
        MethodPossible mp;
        if (line == parser.class_name)
            continue;

        mp.key_name = line;
        mp.possible_names.push_back(line);
        only_main_cpp.push_back(mp);
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
            for (const auto& lineInFile : file.second)
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

    for (const auto& possible_name : only_main_cpp)
    {
        for (const auto& line : files.at(classForParser))
        {
            for (const auto& combination : possible_name.possible_names)
            {
                std::string declaration = "::";
                declaration.append(combination);
                if (line.contains(combination) && !line.contains(declaration))
                {
                    ocurences[possible_name.key_name]++;
                }
            }
        }
    }

    return ocurences;
}



std::unordered_map<std::string, int> Analyzer::GetMethodOcurenceWithoutGlobalPointer(const MethodsVariableCounter& parser, const std::unordered_map<std::string, std::vector<std::string>>& files, const std::string& classForParser)
{
    std::unordered_map<std::string, int> method_occurence;
    // here, one variable should be checked only in .cpp file for methods usage

    for (const auto& hFileLines : m_allFilesH)
    {
        auto hFile = hFileLines.second;
        if (!m_allFilesCPP.contains(hFileLines.first))
            continue;
        auto cppFile = m_allFilesCPP.at(hFileLines.first);

        std::vector<VariableVisibilityRange> variables;

        for (const auto& line : hFile)
        {
            if (line.contains(parser.class_name) && !line.contains('(') && !line.contains(')'))
            {
                int posLeft = 0;
                int posRight = -1;
                bool usable = true;
                int pos = static_cast<int>(line.find(";") - 1);
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
                        break;
                    }

                    auto IsAcceptableSign = [](char a)
                    {
                        return std::isalpha(a) || std::isdigit(a) || a == '_';
                    };

                    while (pos >= 0 && IsAcceptableSign(line[pos]))
                        pos--;

                    std::string substr = line.substr(pos + 1, posRight - pos);
                    VariableVisibilityRange visibility;
                    visibility.variable = substr;
                    visibility.visibility = 0;
                    variables.push_back(visibility);
                    std::string restOfLine = line.substr(0, pos);

                    if (restOfLine.back() - 1 == ',' || restOfLine.back() - 2 == ',' || restOfLine.back() == ',')
                        pos = static_cast<int>(restOfLine.find_last_of(',') - 1);
                    else
                        usable = false;

                } while (usable);
            }
        }

        for (const auto& line : cppFile)
        {
            for (const auto& method : parser.public_methods)
            {
                for (const auto& variable : variables)
                {
                    if (line.contains(variable.variable) && line.contains(method) && method != parser.class_name)
                        method_occurence[method]++;
                }
            }
        }

    }

    return method_occurence;
}
