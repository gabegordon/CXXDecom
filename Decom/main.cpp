#include "stdafx.h"
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
		std::string filename = argv[1];
		std::string instrument = argv[2];
		bool debug = !!atoi(argv[4]);

		DatabaseReader dr(filename);
		Decom decomEngine(instrument, debug, dr.getEntries());
		decomEngine.init(filename);
	}

	//printDataBase();
    return 0;
}

