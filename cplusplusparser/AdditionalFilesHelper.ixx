export module AdditionalFilesHelper;

export import <string>;
export import <set>;
export import <vector>;

export
{
    struct ClassParser
    {
        std::set<std::string> unique_variables;
        std::set<std::string> private_methods;
        std::set<std::string> public_methods;
        std::set<std::string> static_methods;
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

    enum class Visibility
    {
        Private,
        Public
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


}