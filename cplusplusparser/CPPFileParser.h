#pragma once
#include "Parser.h"
class CPPFileParser :
    protected Parser
{
public:
    CPPFileParser();
    std::vector<std::string> RefineFile(const std::vector<std::string>& lines);
};

