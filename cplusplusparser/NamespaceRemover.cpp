#include "NamespaceRemover.h"

std::vector<std::vector<std::string>> NamespaceRemover::ParseLines(std::vector<std::string>& lines)
{
    return FindAllEncapsulations(TypeOfEncapsulation::Namespace, lines);
}
