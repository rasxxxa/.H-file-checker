#include "HFileParser.h"

HFileParser::HFileParser()
{
    // Order does not matter
    __super::RegisterParser(std::make_shared<CommentRemover>());
    __super::RegisterParser(std::make_shared<NamespaceRemover>());
    __super::RegisterParser(std::make_shared<EnumRemover>());
    __super::RegisterParser(std::make_shared<StructRemover>());
    __super::RegisterParser(std::make_shared<EmptyLinesRemover>());
}

std::vector<MethodsVariableCounter> HFileParser::ExtractFromFile(const std::vector<std::string>& lines)
{
    std::vector<std::string> copy_lines = std::vector(lines);
    std::vector<MethodsVariableCounter> counter_per_class;

    __super::DoRefining(copy_lines);

    ClassRemover class_extracter;

    auto classes = class_extracter.ParseLines(copy_lines);

    ClassNameExtracter class_name_extracter;
    VariableExtracter variable_extracter;
    MethodExtracter method_extracter;

    for (auto& _class : classes)
    {
        MethodsVariableCounter counter;
        class_name_extracter.Extract(_class, counter);
        method_extracter.Extract(_class, counter);
        variable_extracter.Extract(_class, counter);
        if (!counter.class_name.empty())
            counter_per_class.push_back(counter);
    }

    return counter_per_class;
}
