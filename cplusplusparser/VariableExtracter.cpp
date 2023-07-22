#include "VariableExtracter.h"

void VariableExtracter::Extract(std::vector<std::string>& lines, MethodsVariableCounter& extracter)
{
	std::vector<std::string> lines_without_variables;
	for (const auto& line : lines)
	{
        std::string variable;
        if (!line.contains(";"))
        {
            lines_without_variables.push_back(line);
            continue;
        }

        bool isExtern = line.find("extern") != std::string::npos;
        int pos = static_cast<int>(line.find(";") - 1);
        int posLeft = 0;
        int posRight = -1;
        bool usable = true;

        do
        {
            while (pos >= 0)
            {
                if (line[pos] == ']')
                {
                    while (pos >= 0 && line[pos] != '[')
                        pos--;
                    pos--;
                }
                else if (line[pos] == '}')
                {
                    while (pos >= 0 && line[pos] != '{')
                        pos--;
                    pos--;
                }
                else
                {
                    posRight = pos;
                    break;
                }
            }

            if (posRight == -1)
            {
                lines_without_variables.push_back(line);
                break;
            }

            auto IsAcceptableSign = [](char a)
            {
                return std::isalpha(a) || std::isdigit(a) || a == '_';
            };

            while (pos >= 0 && IsAcceptableSign(line[pos]))
                pos--;

            std::string substr = line.substr(pos + 1, posRight - pos);
            extracter.unique_variables.insert(substr);
            if (isExtern)
                extracter.global_pointer = substr;
            std::string restOfLine = line.substr(0, pos);

            if (restOfLine.back() - 1 == ',' || restOfLine.back() - 2 == ',' || restOfLine.back() == ',')
                pos = static_cast<int>(restOfLine.find_last_of(',') - 1);
            else
                usable = false;

        } while (usable);
	}
    lines = lines_without_variables;

}
