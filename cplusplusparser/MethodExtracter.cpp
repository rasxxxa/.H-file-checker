#include "MethodExtracter.h"

void MethodExtracter::Extract(std::vector<std::string>& lines, MethodsVariableCounter& extracter)
{
    size_t pos = 0;
    Visibility defaultVis = Visibility::Private;
    std::vector<std::string> withoudMethods;
    while (pos < lines.size())
    {
        if (lines[pos].contains("private:") || lines[pos].contains("protected:"))
            defaultVis = Visibility::Private;
        else if (lines[pos].contains("public:"))
            defaultVis = Visibility::Public;


        if (!lines[pos].contains("("))
        {
            withoudMethods.push_back(lines[pos]);
        }
        else
        {
            bool isStatic = false;
            if (lines[pos].contains("static"))
            {
                isStatic = true;
            }
            auto posOfParentesis = lines[pos].find("(") - 1;
            while (posOfParentesis >= 0 && std::isspace(lines[pos][posOfParentesis])) posOfParentesis--;
            if (posOfParentesis >= 0 && lines[pos][posOfParentesis] == '>')
            {
                posOfParentesis--;
                size_t numberOfRightArrows = 1;
                while (posOfParentesis >= 0 && numberOfRightArrows != 0)
                {
                    if (lines[pos][posOfParentesis] == '<')
                        numberOfRightArrows--;
                    else if (lines[pos][posOfParentesis] == '>')
                        numberOfRightArrows++;
                    
                    posOfParentesis--;
                }
            }
            std::string Method = "";
            while (posOfParentesis >= 0 && lines[pos][posOfParentesis] != ' ')
            {
                Method.push_back(lines[pos][posOfParentesis]);
                posOfParentesis--;
            }
            std::reverse(Method.begin(), Method.end());
            
            const static auto FilterMethod = [](const std::string& line_to_check)
            {
                return !line_to_check.contains("operator") && !line_to_check.contains("=") && !line_to_check.contains("&&") && !line_to_check.contains("~");
            };

            if (FilterMethod(Method))
            {
                const auto it = std::remove_if(Method.begin(), Method.end(), [](char a)
                    {
                        return a == '&' || a == ' ' || a == '*';
                    });
                Method.erase(it, Method.end());
                if (defaultVis == Visibility::Private)
                    extracter.private_methods.insert(Method);
                else
                    extracter.public_methods.insert(Method);

                if (isStatic)
                    extracter.static_methods.insert(Method);
            }

            while (pos < lines.size() && (!lines[pos].contains(");")))
                pos++;
        }
        pos++;
    }
    lines = std::vector<std::string>(withoudMethods);
}
