#include "Parser.h"

void Parser::RegisterParser(const std::shared_ptr<ParserType>& parser)
{
	parsers.push_back(parser);
}

void Parser::DoRefining(std::vector<std::string>& lines)
{
	for (const auto& parser : parsers)
		parser->ParseLines(lines);
}
