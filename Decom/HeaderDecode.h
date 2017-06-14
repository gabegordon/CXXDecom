#pragma once
#include <cstdint>
#include <tuple>
#include "DataTypes.h"

namespace HeaderDecode
{

	const struct DataTypes::PrimaryHeader p_defaults = { DataTypes::STANDALONE, 0, 0, 0, 0 };
	const struct DataTypes::SecondaryHeader s_defaults = {0,0};

	std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> decodeHeaders(std::ifstream& infile);

	DataTypes::PrimaryHeader decodePrimary(std::ifstream& infile);

	DataTypes::SecondaryHeader decodeSecondary(std::ifstream& infile);

	void checkValidHeader(const DataTypes::PrimaryHeader& pheader);

}
