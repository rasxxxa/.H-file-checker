export module AdditionalFilesHelper;

export import <string>;
export import <iostream>;
export import <set>;
export import <vector>;
export import <regex>;
export import <unordered_map>;
export import <cassert>;
export import <filesystem>;
export import <map>;
export import <fstream>;
export import <string_view>;
export import <algorithm>;
export import <sstream>;

export
{
    struct VariableVisibilityRange
    {
        std::string variable;
        size_t visibility;
    };
    struct VariableUsage
    {
        std::string variable;
        size_t usage;
    };

    struct Files
    {
        std::string h_File;
        std::string cpp_File;
    };

    struct MethodsVariableCounter
    {
        std::set<std::string> private_methods;
        std::set<std::string> public_methods;
        std::set<std::string> static_methods;
        std::set<std::string> unique_variables;
        std::string class_name;
        std::string global_pointer;
    };

    struct VariableOccurrenceReadWrite
    {
        size_t read_times;
        size_t write_times;
        size_t method_times;
        constexpr bool operator ==(const VariableOccurrenceReadWrite& other) const
        {
            return read_times == other.read_times && write_times == other.write_times && method_times == other.method_times;
        }
    };

    inline bool IsOnlySpaces(const std::string& line)
    {
        return std::all_of(line.begin(), line.end(), [](const char a) {return std::isspace(a); });
    }

    enum class Visibility
    {
        Private,
        Public
    };

    enum class TypeOfEncapsulation
    {
        Struct,
        Class,
        Namespace,
        Enum
    };

    std::unordered_map<TypeOfEncapsulation, const std::string> mapped_encapsulation
    {
        { TypeOfEncapsulation::Struct, "struct" },
        { TypeOfEncapsulation::Class, "class" },
        { TypeOfEncapsulation::Namespace, "namespace" },
        { TypeOfEncapsulation::Enum, "enum" },
    };

    struct Parenthesis
    {
        size_t left{ 0 };
        size_t right{ 0 };
    };

    template <typename T>
    concept IsVectorOrArray = requires(T a)
    {
        a.begin();
        a.end();
        T::value_type;
    };

    template <typename Func, typename In, typename... Args>
    constexpr bool RunEqualTests(In argTest, Func func, Args... args)
    {
        if constexpr (std::is_integral_v<decltype(func(std::forward<Args>(args)...))> || std::is_same_v<decltype(func(std::forward<Args>(args)...)), VariableOccurrenceReadWrite>)
        {
            return (func(std::forward<Args>(args)...) == argTest);
        }
        else if constexpr (std::is_same_v<decltype(func(std::forward<Args>(args)...)), float>)
        {
            return (abs(func(std::forward<Args>(args)...) - argTest) <= std::numeric_limits<float>::epsilon());
        }
        else if constexpr (IsVectorOrArray<decltype(func(std::forward<Args>(args)...))>)
        {
            auto val = func(std::forward<Args>(args)...);
            return std::equal(std::begin(val), std::end(val), std::begin(argTest), std::end(argTest), [](const auto& val1, const auto& val2)
                {
                    return abs(float(val1) - float(val2)) <= std::numeric_limits<float>::epsilon();
                });
        }

        return false;
    }

    Parenthesis CountParenthesis(const std::string& line)
    {
        Parenthesis parenthesis;
        for (const auto& character : line)
        {
            if (character == '{')
                parenthesis.left++;
            else if (character == '}')
                parenthesis.right++;
        }

        return parenthesis;
    }
    
    inline bool IsElement(const std::string& line, TypeOfEncapsulation encapsulation)
    {
        switch (encapsulation)
        {
        case TypeOfEncapsulation::Struct:
        {
            std::regex pattern("\\bstruct\\b");
            return std::regex_search(line, pattern);
        }
        case TypeOfEncapsulation::Class:
        {
            std::regex pattern("\\bclass\\b");
            return std::regex_search(line, pattern);
        }
        case TypeOfEncapsulation::Namespace:
        {
            std::regex pattern("\\bnamespace\\b");
            return std::regex_search(line, pattern);
        }
        case TypeOfEncapsulation::Enum:
        {
            std::regex pattern("\\benum\\b");
            return std::regex_search(line, pattern);
        }
        default:
        {
            assert(false);
            return false;
        }
        }
    }

    std::vector<std::vector<std::string>> FindAllEncapsulations(TypeOfEncapsulation encapsulation, std::vector<std::string>& lines)
    {
        std::vector<std::vector<std::string>> elements;
        std::vector<std::vector<std::string>> elements_temp;
        std::vector<std::string> other_lines;
        size_t pos = 0;
        bool any_elements_found = false;
        size_t elements_number = 0;
        size_t opened_parenthesis = 0;
        size_t elements_found_number = 0;
        while (pos < lines.size())
        {
            if (IsElement(lines[pos], encapsulation))
            {
                std::vector<std::string> namespace_found;
                namespace_found.push_back(lines[pos]);
                elements_temp.push_back(namespace_found);
                any_elements_found = true;
                elements_number++;
                elements_found_number++;
                Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
                opened_parenthesis += parenthesis_number.left;
                opened_parenthesis -= parenthesis_number.right;
                if (opened_parenthesis < elements_number && parenthesis_number.left > 0)
                {
                    elements.push_back(elements_temp.back());
                    elements_temp.pop_back();

                    if (!--elements_number)
                        any_elements_found = false;
                }
            }
            else if (any_elements_found)
            {
                elements_temp[elements_temp.size() - 1].push_back(lines[pos]);
                Parenthesis parenthesis_number = CountParenthesis(lines[pos]);
                opened_parenthesis += parenthesis_number.left;
                opened_parenthesis -= parenthesis_number.right;
                if (opened_parenthesis < elements_number)
                {
                    elements.push_back(elements_temp.back());
                    elements_temp.pop_back();

                    if (!--elements_number)
                        any_elements_found = false;
                }
            }
            else if (encapsulation == TypeOfEncapsulation::Class && lines[pos].find("extern") != std::string::npos)
            {
                elements[elements.size() - 1].push_back(lines[pos]);
            }
            else
            {
                other_lines.push_back(lines[pos]);
            }
            pos++;
        }

        lines = std::vector(other_lines);
        return elements;
    }
}