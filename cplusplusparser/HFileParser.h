#pragma once
#include "Parser.h"
class HFileParser :
    protected Parser
{
public:
    HFileParser();
    std::vector<MethodsVariableCounter> ExtractFromFile(const std::vector<std::string>&lines);
};

