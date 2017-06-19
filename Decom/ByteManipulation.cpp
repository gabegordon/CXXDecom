#include "stdafx.h"
#include "ByteManipulation.h"

namespace ByteManipulation {
	std::ostream &operator<<(std::ostream &os, char c) {
		return os << (std::is_signed<char>::value ? static_cast<int>(c)
			: static_cast<unsigned int>(c));
	}

	std::ostream &operator<<(std::ostream &os, signed char c) {
		return os << static_cast<int>(c);
	}

	std::ostream &operator<<(std::ostream &os, unsigned char c) {
		return os << static_cast<unsigned int>(c);
	}

	uint16_t swapEndian16(const uint16_t& val)
	{
		return _byteswap_ushort(val);
	}
	uint32_t swapEndian32(const uint32_t& val)
	{
		return _byteswap_ulong(val);
	}

	uint64_t swapEndian64(const uint64_t& val)
	{
		return _byteswap_uint64(val);
	}

	uint32_t extract8(const uint8_t& val, uint32_t start, uint32_t len)
	{
		return std::stoul((std::bitset<8>(val).to_string().substr(start, len)), nullptr, 2);
	}

	uint32_t extract16(const uint16_t& val, uint32_t start, uint32_t len)
	{
		return std::stoul((std::bitset<16>(val).to_string().substr(start, len)), nullptr, 2);
	}

	uint32_t extract32(const uint32_t& val, uint32_t start, uint32_t len)
	{
		return std::stoul((std::bitset<32>(val).to_string().substr(start, len)), nullptr, 2);
	}

	uint64_t extract64(const uint64_t& val, uint32_t start, uint32_t len)
	{
		return std::stoul((std::bitset<64>(val).to_string().substr(start, len)), nullptr, 2);
	}

	int32_t extract8Signed(const int8_t& val, uint32_t start, uint32_t len)
	{
		return std::stol((std::bitset<8>(val).to_string().substr(start, len)), nullptr, 2);
	}

	int32_t extract16Signed(const uint16_t& val, uint32_t start, uint32_t len)
	{
		return std::stol((std::bitset<16>(val).to_string().substr(start, len)), nullptr, 2);
	}
	int32_t extract32Signed(const uint32_t& val, uint32_t start, uint32_t len)
	{
		return std::stol((std::bitset<32>(val).to_string().substr(start, len)), nullptr, 2);
	}

	int64_t extract64Signed(const uint64_t& val, uint32_t start, uint32_t len)
	{
		return std::stoll((std::bitset<64>(val).to_string().substr(start, len)), nullptr, 2);
	}

	uint32_t mergeBytes(uint8_t& initialByte, uint8_t& extraByte1, uint8_t& extraByte2, uint8_t& extraByte3, const uint32_t& num)
	{
		std::string b1 = std::bitset<8>(initialByte).to_string();
		std::string b2 = std::bitset<8>(extraByte1).to_string();
		std::string b3 = std::bitset<8>(extraByte2).to_string();
		std::string b4 = std::bitset<8>(extraByte3).to_string();
		std::string s_result;
		if (num > 3)
		{
			s_result = b1 + b2 + b3 + b4;
		}
		else
		{
			s_result = b1 + b2 + b3;
		}
		return std::stoul(s_result, nullptr, 2);
	}
}