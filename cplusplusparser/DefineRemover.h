#pragma once
#include "ParserType.h"
class DefineRemover :
    public ParserType
{
public:
    virtual std::vector<std::vector<std::string>> ParseLines(std::vector<std::string>& lines) override;

    DefineRemover() = default;
};

