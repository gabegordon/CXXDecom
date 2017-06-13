#pragma once
#include <iostream>
#include <typeinfo>
#include <cinttypes>
#include <vector>

namespace ByteManipulation {
	inline std::ostream &operator<<(std::ostream &os, char c) {
		return os << (std::is_signed<char>::value ? static_cast<int>(c)
			: static_cast<unsigned int>(c));
	}

	inline std::ostream &operator<<(std::ostream &os, signed char c) {
		return os << static_cast<int>(c);
	}

	inline std::ostream &operator<<(std::ostream &os, unsigned char c) {
		return os << static_cast<unsigned int>(c);
	}


	inline uint16_t mergeBytes(const uint8_t& a, const uint8_t& b)
	{
		return (a << 8) | (b & 0xff);
	}

	std::vector<uint8_t> splitBytes(uint16_t bytes)
	{
		std::vector<uint8_t> arr(2);
		arr.at(0) = bytes & 0xff;
		arr.at(1) = bytes >> 8;
		return arr;
	}
}