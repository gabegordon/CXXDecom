#include <iostream>
#include <cstdint>
#include <string>
#include <chrono>
#include "Decom.h"
#include "DatabaseReader.h"

int main(int argc, char* argv[])
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    if (argc < 5)
    {
        std::cout << "Specify: database and instrument\n";
        return 0;
    }
    else
    {
        std::string instrument = argv[1];
        std::string packetFile = argv[2];
        std::string paramsFile = argv[3];
        bool debug = false;
        bool allAPIDs = !!std::stoi(argv[4]);
        std::cout << packetFile << std::endl;
        system("cd output && del /Q *.txt 2>NUL 1>NUL");
        DatabaseReader dr(paramsFile, allAPIDs);
        Decom decomEngine(instrument, debug, dr.getEntries());
        decomEngine.init(packetFile);
    }
    std::cout << std::endl;
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout  << "elapsed time: " << elapsed_seconds.count() << "s\n";
    system("pause");
    return 0;
}
