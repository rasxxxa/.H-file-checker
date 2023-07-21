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
    m_allFiles.clear();
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
        
        m_allFiles[file.h_File] = linesCpp;
        m_mapped_parsers[file.h_File] = extracted;
    }

    for (const auto& check_all_parsers : m_mapped_parsers)
    {
        for (const auto& parser : check_all_parsers.second)
        {
            auto occurrenceVarsReadWrite = CheckVariablesForUsage(parser.unique_variables, m_allFiles[check_all_parsers.first]);
            for (const auto& variable : occurrenceVarsReadWrite)
                m_usage_per_file_variables[check_all_parsers.first][parser.class_name] = variable.second;

            auto occurrence = GetOccurenceOfVariables(parser, m_allFiles[check_all_parsers.first]);
            for (const auto& variable : occurrence)
                m_occurence_per_file_variables[check_all_parsers.first][parser.class_name] = variable.second;

            auto usageMethods = GetMethodOcurence(parser, m_allFiles, check_all_parsers.first);
            for (const auto& method : usageMethods)
                m_usage_per_file_methods[check_all_parsers.first][parser.class_name] = method.second;
        }
    }
}

void Analyzer::PrintResults(const std::string& path)
{

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
