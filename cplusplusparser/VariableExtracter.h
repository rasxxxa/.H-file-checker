#pragma once
#include "ElementExtracter.h"
class VariableExtracter final
	: public ElementExtracter
{
public:
	// For clearest results, call after namespace, struct, enum remover, and method extracted
	virtual void Extract(std::vector<std::string>& lines, MethodsVariableCounter& extracter) override;
};

