#include "ClassRemover.h"

std::vector<std::vector<std::string>> ClassRemover::ParseLines(std::vector<std::string>& lines)
{
    return FindAllEncapsulations(TypeOfEncapsulation::Class, lines);
}
