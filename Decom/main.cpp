#include "stdafx.h"
#include <iostream>
#include <cstdint>
#include "Decom.h"
#include "DatabaseReader.h"

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{	
	if (argc < 4)
	{
		cout << "Specify: database and instrument\n";
		return 0;
	}
	else
	{
		std::string databaseFile = argv[1];
		std::string instrument = argv[2];
		std::string packetFile = argv[3];
		bool debug = true;

		DatabaseReader dr(databaseFile);
		Decom decomEngine(instrument, debug, dr.getEntries());
		decomEngine.init(packetFile);
	}

	//printDataBase();
    return 0;
}

