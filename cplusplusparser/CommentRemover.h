#pragma once
#include "ParserType.h"
class CommentRemover :
    public ParserType
{
public:
    // returns comment lines
    // accepts all lines with comments
    virtual std::vector<std::vector<std::string>> ParseLines(std::vector<std::string>& lines) override;
};

