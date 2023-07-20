#pragma once
#include "ElementExtracter.h"
class ClassNameExtracter :
    public ElementExtracter
{
public:
    //One one class file should be called
    virtual void Extract(std::vector<std::string>& lines, MethodsVariableCounter& extracter) override;
};

