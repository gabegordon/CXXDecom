#include <iostream>
#include <cstdint>
#include "Decom.h"
#include "DatabaseReader.h"

using std::cout;
using std::endl;
int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        cout << "Specify: database and instrument\n";
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
    system("pause");
    return 0;
}
