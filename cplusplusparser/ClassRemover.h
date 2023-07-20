#pragma once
#include "ParserType.h"
class ClassRemover final:
    public ParserType
{
public:
    // Comments should be removed before calling this one
    virtual std::vector<std::vector<std::string>> ParseLines(std::vector<std::string>& lines) override;
};

