#pragma once
#include "Parser.h"
class HFileParser :
    protected Parser
{
public:
    HFileParser();
    std::unordered_map<std::string, MethodsVariableCounter> ExtractFromFile(const std::vector<std::string>&lines);
};

