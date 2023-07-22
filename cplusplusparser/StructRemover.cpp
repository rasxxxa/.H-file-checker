#include "StructRemover.h"

std::vector<std::vector<std::string>> StructRemover::ParseLines(std::vector<std::string>& lines)
{
	return FindAllEncapsulations(TypeOfEncapsulation::Struct, lines);
}
