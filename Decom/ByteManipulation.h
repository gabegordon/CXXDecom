#pragma once
#include <iostream>
#include <typeinfo>
#include <cinttypes>
#include <bitset>
#include <string>

namespace ByteManipulation {
	std::ostream &operator<<(std::ostream &os, char c);

	std::ostream &operator<<(std::ostream &os, signed char c);

	std::ostream &operator<<(std::ostream &os, unsigned char c);

	uint16_t swapEndian16(const uint16_t& val);
	
	uint32_t swapEndian32(const uint32_t& val);

	uint64_t swapEndian64(const uint64_t& val);

	uint32_t extract8(const uint8_t& val, uint32_t start, uint32_t len);

	uint32_t extract16(const uint16_t& val, uint32_t start, uint32_t len);

	uint32_t extract32(const uint32_t& val, uint32_t start, uint32_t len);

	uint64_t extract64(const uint64_t& val, uint32_t start, uint32_t len);

	int32_t extract8Signed(const int8_t& val, uint32_t start, uint32_t len);

	int32_t extract16Signed(const uint16_t& val, uint32_t start, uint32_t len);

	int32_t extract32Signed(const uint32_t& val, uint32_t start, uint32_t len);

	int64_t extract64Signed(const uint64_t& val, uint32_t start, uint32_t len);

	uint32_t mergeBytes(uint8_t& initialByte, uint8_t& extraByte1, uint8_t& extraByte2, uint8_t& extraByte3, const uint32_t& num);

	static inline uint32_t mergeBytes16(uint8_t& initialByte, uint8_t& extraByte1)
	{
		std::string b1 = std::bitset<8>(initialByte).to_string();
		std::string b2 = std::bitset<8>(extraByte1).to_string();
		std::string s_result = b1 + b2;
		return std::stoul(s_result, nullptr, 2);
	}
}