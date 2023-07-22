#pragma once
import AdditionalFilesHelper;

class ElementExtracter // Class, method, variables
{
protected:
	virtual void Extract(std::vector<std::string>& lines, MethodsVariableCounter& extracter) = 0;
};

