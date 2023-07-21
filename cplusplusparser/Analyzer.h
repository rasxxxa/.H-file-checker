#pragma once
import AdditionalFilesHelper;
#include "File.hpp"
#include "CPPFileParser.h"
#include "HFileParser.h"

class Analyzer final
{
public:
	Analyzer(const std::string& path = ".");
	void Analize();
	void PrintResults(const std::string& path = "UnusedVariables.txt");
private:
	std::unordered_map<std::string, int> GetOccurenceOfVariables(const MethodsVariableCounter& parser, const std::vector<std::string>& lines);
	std::unordered_map<std::string, VariableOccurrenceReadWrite> CheckVariablesForUsage(const std::set<std::string>& variables, const std::vector<std::string>& sourceFile);
	std::unordered_map<std::string, int> GetMethodOcurence(const MethodsVariableCounter& parser, const std::unordered_map<std::string, std::vector<std::string>>& files, const std::string& classForParser);
	CPPFileParser m_cppParser;
	HFileParser m_hParser;
	std::map<std::string, std::vector<MethodsVariableCounter>> m_mapped_parsers;
	std::vector<Files> m_files_to_check;
	std::filesystem::path m_path;
	std::unordered_map<std::string, std::vector <std::string>> m_allFiles;
	std::unordered_map<std::string, std::unordered_map<std::string, VariableOccurrenceReadWrite>> m_usage_per_file_variables;
	std::unordered_map<std::string, std::unordered_map<std::string, int>> m_occurence_per_file_variables;
	std::unordered_map<std::string, std::unordered_map<std::string, int>> m_usage_per_file_methods;
};

