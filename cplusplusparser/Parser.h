#pragma once
import AdditionalFilesHelper;
#include "ParserType.h"
#include "CommentRemover.h"
#include "EnumRemover.h"
#include "NamespaceRemover.h"
#include "EmptyLinesRemover.h"
#include "StructRemover.h"
#include "ClassRemover.h"
#include "ClassNameExtracter.h"
#include "VariableExtracter.h"
#include "MethodExtracter.h"

class Parser
{
public:
	void DoRefining(std::vector<std::string>& lines);
protected:
	void RegisterParser(const std::shared_ptr<ParserType>& parser);
	std::vector<std::shared_ptr<ParserType>> parsers;
};

