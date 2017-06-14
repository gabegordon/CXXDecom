#pragma once
#include <iostream>
#include <typeinfo>
#include <cinttypes>
#include <bitset>

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

	static inline uint16_t swapEndian16(const uint16_t& val)
	{
		return _byteswap_ushort(val);
	}
	static inline uint32_t swapEndian32(const uint32_t& val)
	{
		return _byteswap_ulong(val);
	}

	static inline uint64_t swapEndian64(const uint64_t& val)
	{
		return _byteswap_uint64(val);
	}
	
	static inline uint32_t extract8(const uint8_t& val, uint32_t start, uint32_t len)
	{
		return std::stoul((std::bitset<8>(val).to_string().substr(start, len)), nullptr, 2);
	}

	static inline uint32_t extract16(const uint16_t& val, uint32_t start, uint32_t len)
	{
		return std::stoul((std::bitset<16>(val).to_string().substr(start, len)), nullptr, 2);
	}

	static inline uint32_t extract32(const uint32_t& val, uint32_t start, uint32_t len)
	{
		return std::stoul((std::bitset<32>(val).to_string().substr(start, len)), nullptr, 2);
	}

	static inline uint64_t extract64(const uint64_t& val, uint32_t start, uint32_t len)
	{
		return std::stoul((std::bitset<64>(val).to_string().substr(start, len)), nullptr, 2);
	}

	static inline int32_t extract8Signed(const int8_t& val, uint32_t start, uint32_t len)
	{
		return std::stol((std::bitset<8>(val).to_string().substr(start, len)), nullptr, 2);
	}

	static inline int32_t extract16Signed(const uint16_t& val, uint32_t start, uint32_t len)
	{
		return std::stol((std::bitset<16>(val).to_string().substr(start, len)), nullptr, 2);
	}

	static inline int32_t extract32Signed(const uint32_t& val, uint32_t start, uint32_t len)
	{
		return std::stol((std::bitset<32>(val).to_string().substr(start, len)), nullptr, 2);
	}

	static inline int64_t extract64Signed(const uint64_t& val, uint32_t start, uint32_t len)
	{
		return std::stoll((std::bitset<64>(val).to_string().substr(start, len)), nullptr, 2);
	}
}