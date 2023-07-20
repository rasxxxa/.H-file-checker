#include "ClassNameExtracter.h"

void ClassNameExtracter::Extract(std::vector<std::string>& lines, MethodsVariableCounter& extracter)
{
    for (const auto& line : lines)
    {
        if (IsElement(line, TypeOfEncapsulation::Class))
        {
            auto classPos = line.find(mapped_encapsulation[TypeOfEncapsulation::Class]);

            size_t pos = line.find_first_not_of(mapped_encapsulation[TypeOfEncapsulation::Class], classPos);
            std::string className;
            if (pos != std::string::npos)
            {
                while (pos != line.size() && (!std::isalpha(line[pos])) || line[pos] == '_') pos++;
                while (std::isalpha(line[pos]) || line[pos] == '_') { className.push_back(line[pos]); pos++; }
                if (!className.empty())
                {
                    extracter.class_name = className;
                    return;
                }
            }
        }
    }
}
