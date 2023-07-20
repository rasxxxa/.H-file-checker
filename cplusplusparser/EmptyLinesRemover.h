#pragma once
#include "ParserType.h"
class EmptyLinesRemover :
    public ParserType
{
public:
    // Returning only one dimension of vector
    virtual std::vector<std::vector<std::string>> ParseLines(std::vector<std::string>& lines) override;
};

