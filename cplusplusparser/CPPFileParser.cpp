#include "CPPFileParser.h"

#include "DefineRemover.h"

CPPFileParser::CPPFileParser()
{
    // Order does not matter
    __super::RegisterParser(std::make_shared<CommentRemover>());
    __super::RegisterParser(std::make_shared<DefineRemover>());
    __super::RegisterParser(std::make_shared<NamespaceRemover>());
    __super::RegisterParser(std::make_shared<EmptyLinesRemover>());
    __super::RegisterParser(std::make_shared<StructRemover>());
}

std::vector<std::string> CPPFileParser::RefineFile(const std::vector<std::string>& lines)
{
    std::vector<std::string> refined_lines = std::vector(lines);

    __super::DoRefining(refined_lines);

    return refined_lines;
}
