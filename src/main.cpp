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
#ifdef __linux__
        (void) system("cd output && rm -f *.txt");  // Clear output directory
#else
        (void) system("cd output && del /Q *.txt 2>NUL 1>NUL");
#endif
        DatabaseReader dr(paramsFile, allAPIDs);  // Read databases
        Decom decomEngine(instrument, debug, dr.getEntries());  // Run decom
        decomEngine.init(packetFile);
    }
    std::cout << std::endl;
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout  << "elapsed time: " << elapsed_seconds.count() << "s\n";
    (void) system("pause");
    return 0;
}
