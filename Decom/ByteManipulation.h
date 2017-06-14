#pragma once
#include <iostream>
#include <typeinfo>
#include <cinttypes>
#include <vector>

typedef uint8_t BYTE;

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

	inline int16_t mergeBytesSigned(const int8_t& a, const int8_t& b)
	{
		return (a << 8) | (b & 0xff);
	}

	inline uint32_t mergeShorts(const uint16_t& a, const uint16_t& b)
	{
		return (a << 16) | (b & 0xff);
	}

	inline int32_t mergeShortsSigned(const int16_t& a, const int16_t& b)
	{
		return (a << 16) | (b & 0xff);
	}
	
	inline float mergeInts(const int32_t& a, const int32_t& b)
	{
		return (a << 32) | (b & 0xff);
	}


	std::vector<BYTE> splitBytes(uint16_t bytes)
	{
		std::vector<BYTE> arr(2);
		arr.at(0) = bytes & 0xff;
		arr.at(1) = bytes >> 8;
		return arr;
	}
}