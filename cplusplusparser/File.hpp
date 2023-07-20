#pragma once
#include <vector>
#include <string>
#include <fstream>
class File final
{
public:
	File() = delete;
	File(const File& file) = delete;
	File operator=(const File& file) = delete;
	static std::vector<std::string> ReadFile(const std::string_view& path);
	static void WriteToFile(const std::string_view& path, const std::vector<std::string>& line);
};



inline std::vector<std::string> File::ReadFile(const std::string_view& path)
{
    std::vector<std::string> file;
    std::ifstream file_opened(path.data());
    if (file_opened.is_open())
    {
        std::string line;
        while (std::getline(file_opened, line))
        {
            file.push_back(line);
        }

        file_opened.close();
    }

    return file;
}

inline void File::WriteToFile(const std::string_view& path, const std::vector<std::string>& lines)
{
    std::ofstream file_to_write(path.data());
    if (file_to_write.is_open())
    {
        for (const auto& line : lines)
            file_to_write << line << std::endl;

        file_to_write.flush();
        file_to_write.close();
    }
}

