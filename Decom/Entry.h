#pragma once
#include <string>
struct entry {
	std::string mnemonic;
	std::string description;
	std::string SS;
	std::string type;
	std::string s_APID;
	uint32_t i_APID;
	int32_t byte;
	int32_t bitLower;
	int32_t bitUpper;
};