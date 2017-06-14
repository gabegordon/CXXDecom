#pragma once
#include <string>
struct entry {
	std::string mnemonic;
	std::string description;
	std::string SS;
	std::string type;
	std::string s_APID;
	uint32_t i_APID;
	uint32_t byte;
	uint32_t bitLower;
	uint32_t bitUpper;
};

const struct entry defaults = { 
	"","","","","",0,0,0,0
};