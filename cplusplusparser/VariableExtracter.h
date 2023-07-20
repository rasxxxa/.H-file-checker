#pragma once
#include "VariableMethodExtracter.h"
class VariableExtracter final
	: public VariableMethodExtracter
{
public:
	// For clearest results, call after namespace, struct, enum remover, and method extracted
	virtual void Extract(std::vector<std::string>& lines, MethodsVariableCounter& extracter) override;
};

