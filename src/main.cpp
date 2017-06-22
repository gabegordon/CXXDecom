#include <iostream>
#include <cstdint>
#include "Decom.h"
#include "DatabaseReader.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    if (argc < 6)
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
        bool debug = true;
        bool allAPIDs = !!std::stoi(argv[5]);
        std::cout << packetFile << std::endl;
        DatabaseReader dr(databaseFile, paramsFile, allAPIDs);
        Decom decomEngine(instrument, debug, dr.getEntries());
        decomEngine.init(packetFile);
    }
    //printDataBase();
    return 0;
}
