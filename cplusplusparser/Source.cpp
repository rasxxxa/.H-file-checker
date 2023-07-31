#include <iostream>
#include "Analyzer.h"
#include "File.hpp"

int main(int argc, const char** argv) 
{
    std::string path = "D:\\magicnhd\\Reel5CO\\";
    if (argc > 1)
        path = argv[1];

    Analyzer analizer(path);
    analizer.Analize();
    analizer.PrintResults("Unusable.txt");
}
