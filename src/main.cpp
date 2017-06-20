#include <iostream>
#include <cstdint>
#include <chrono>
#include "Decom.h"
#include "DatabaseReader.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    auto start = std::chrono::high_resolution_clock::now();
    if (argc < 5)
    {
        cout << "Specify: database and instrument\n";
        return 0;
    }
    else
    {
        std::string databaseFile = argv[1];
        std::string instrument = argv[2];
        std::string packetFile = argv[3];
        std::string paramsFile = argv[4];
        bool debug = false;
        bool allAPIDs = true;
        std::cout << packetFile << std::endl;
        DatabaseReader dr(databaseFile, paramsFile, allAPIDs);
        Decom decomEngine(instrument, debug, dr.getEntries());
        decomEngine.init(packetFile);
    }

    //printDataBase();
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    return 0;
}
