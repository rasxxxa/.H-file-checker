#pragma once
import AdditionalFilesHelper;

class VariableMethodExtracter
{
protected:
	virtual void Extract(std::vector<std::string>& lines, MethodsVariableCounter& extracter) = 0;
};

