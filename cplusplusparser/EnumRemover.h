#pragma once
#include "ParserType.h"
class EnumRemover final:
    public ParserType
{
public:
    // Comments should be removed before calling this one, also, structs, inner classes, namespaces...
    // This method should be called only on one class file
    virtual std::vector<std::vector<std::string>> ParseLines(std::vector<std::string>& lines) override;
};

