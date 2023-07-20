#include "EnumRemover.h"

std::vector<std::vector<std::string>> EnumRemover::ParseLines(std::vector<std::string>& lines)
{
    return FindAllEncapsulations(TypeOfEncapsulation::Enum, lines);
}
