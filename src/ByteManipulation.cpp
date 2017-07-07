#include <string>
#include <bitset>
#include "ByteManipulation.h"

namespace ByteManipulation {
/**
 * Stream operator for char. Allows cout to interpret bytes as integer values.
 *
 * @param os Output stream
 * @param c Character to output
 * @return Output stream
 */
std::ostream &operator<<(std::ostream &os, char c) {
    return os << (std::is_signed<char>::value ? static_cast<int>(c)
                  : static_cast<unsigned int>(c));
}

/**
 * Stream operator for signed char. Allows cout to interpret bytes as integer values.
 *
 * @param os Output stream
 * @param c Character to output
 * @return Output stream
 */
std::ostream &operator<<(std::ostream &os, signed char c) {
    return os << static_cast<int>(c);
}

/**
 * Stream operator for unsigned char. Allows cout to interpret bytes as integer values.
 *
 * @param os Output stream
 * @param c Character to output
 * @return Output stream
 */
std::ostream &operator<<(std::ostream &os, unsigned char c) {
    return os << static_cast<unsigned int>(c);
}

/**
 * Swap endian of a 2-byte value. Uses native MSVC method.
 *
 * @param val 16-bit Unsigned Integer
 * @return 16-bit Unsigned Integer (Swapped)
 */
uint16_t swapEndian16(const uint16_t& val)
{
#ifdef __linux__
    return __builtin_bswap16(val);
#else
    return _byteswap_ushort(val);
#endif
}

/**
 * Swap endian of a 4-byte value. Uses native MSVC method.
 *
 * @param val 32-bit Unsigned Integer
 * @return 32-bit Unsigned Integer (Swapped)
 */
uint32_t swapEndian32(const uint32_t& val)
{
#ifdef __linux__
    return __builtin_bswap32(val);
#else
    return _byteswap_ulong(val);
#endif
}

/**
 * Swap endian of a 8-byte value. Uses native MSVC method.
 *
 * @param val 64-bit Unsigned Integer
 * @return 64-bit Unsigned Integer (Swapped)
 */
uint64_t swapEndian64(const uint64_t& val)
{
#ifdef __linux__
    return __builtin_bswap64(val);
#else
    return _byteswap_uint64(val);
#endif
}

/**
 * Extract a range of bits from a single byte.
 *
 * @param val Unsigned 8-bit integer to extract from
 * @param start Starting bit (inclusive)
 * @param len Number of bits to extract
 * @return Extracted bits
 */
uint32_t extract8(const uint8_t& val, const uint32_t& start, const uint32_t& len)
{
    return (val >> start) & ((1 << (len - start)) - 1);
}

/**
 * Extract a range of bits from two bytes.
 *
 * @param val Unsigned 16-bit integer to extract from
 * @param start Starting bit (inclusive)
 * @param len Number of bits to extract
 * @return Extracted bits
 */
uint32_t extract16(const uint16_t& val, const uint32_t& start, const uint32_t& len)
{
    return std::stoul((std::bitset<16>(val).to_string().substr(start, len)), nullptr, 2);
}

/**
 * Extract a range of bits from four bytes.
 *
 * @param val Unsigned 32-bit integer to extract from
 * @param start Starting bit (inclusive)
 * @param len Number of bits to extract
 * @return Extracted bits
 */
uint32_t extract32(const uint32_t& val, const uint32_t& start, const uint32_t& len)
{
    return std::stoul((std::bitset<32>(val).to_string().substr(start, len)), nullptr, 2);
}

/**
 * Extract a range of bits from eight bytes.
 *
 * @param val Unsigned 64-bit integer to extract from
 * @param start Starting bit (inclusive)
 * @param len Number of bits to extract
 * @return Extracted bits
 */
uint64_t extract64(const uint64_t& val, const uint32_t& start, const uint32_t& len)
{
    return std::stoul((std::bitset<64>(val).to_string().substr(start, len)), nullptr, 2);
}

/**
 * Extract a range of bits from a single byte.
 *
 * @param val 8-bit integer to extract from
 * @param start Starting bit (inclusive)
 * @param len Number of bits to extract
 * @return Extracted bits (signed)
 */
int32_t extract8Signed(const uint8_t& val, const uint32_t& start, const uint32_t& len)
{
    return std::stol((std::bitset<8>(val).to_string().substr(start, len)), nullptr, 2);
}

/**
 * Extract a range of bits from two bytes.
 *
 * @param val 16-bit integer to extract from
 * @param start Starting bit (inclusive)
 * @param len Number of bits to extract
 * @return Extracted bits (signed)
 */
int32_t extract16Signed(const uint16_t& val, const uint32_t& start, const uint32_t& len)
{
    return std::stol((std::bitset<16>(val).to_string().substr(start, len)), nullptr, 2);
}

/**
 * Extract a range of bits from four bytes.
 *
 * @param val 32-bit integer to extract from
 * @param start Starting bit (inclusive)
 * @param len Number of bits to extract
 * @return Extracted bits (signed)
 */
int32_t extract32Signed(const uint32_t& val, const uint32_t& start, const uint32_t& len)
{
    return std::stol((std::bitset<32>(val).to_string().substr(start, len)), nullptr, 2);
}

/**
 * Extract a range of bits from eight bytes.
 *
 * @param val 64-bit integer to extract from
 * @param start Starting bit (inclusive)
 * @param len Number of bits to extract
 * @return Extracted bits (signed)
 */
int64_t extract64Signed(const uint64_t& val, const uint32_t& start, const uint32_t& len)
{
    return std::stoll((std::bitset<64>(val).to_string().substr(start, len)), nullptr, 2);
}

/**
 * Merge a variable number of bytes into a larger format.
 *
 * @param initialByte Unsigned 8-bit integer
 * @param extraByte1  Unsigned 8-bit integer
 * @param extraByte2  Unsigned 8-bit integer
 * @param extraByte3  Unsigned 8-bit integer
 * @param num Either 3 or 4, denotes whether merging 3 bytes or 4 bytes
 * @return Unsigned 32-bit integer containing the merged bytes
 */
uint32_t mergeBytes(const uint8_t& initialByte, const uint8_t& extraByte1, const uint8_t& extraByte2, const uint8_t& extraByte3, const uint32_t& num)
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

/**
 * Merge eight bytes into a single 64-bit integer.
 *
 * @param b0 Unsigned 8-bit integer
 * @param b1 Unsigned 8-bit integer
 * @param b2 Unsigned 8-bit integer
 * @param b3 Unsigned 8-bit integer
 * @param b4 Unsigned 8-bit integer
 * @param b5 Unsigned 8-bit integer
 * @param b6 Unsigned 8-bit integer
 * @param b7 Unsigned 8-bit integer
 * @return Unsigned 64-bit integer containing the merged bytes
 */
uint64_t mergeBytes64(const uint8_t& b0, const uint8_t& b1, const uint8_t& b2, const uint8_t& b3, const uint8_t& b4, const uint8_t& b5, const uint8_t& b6, const uint8_t& b7)
{
    std::string b0s = std::bitset<8>(b0).to_string();
    std::string b1s = std::bitset<8>(b1).to_string();
    std::string b2s = std::bitset<8>(b2).to_string();
    std::string b3s = std::bitset<8>(b3).to_string();
    std::string b4s = std::bitset<8>(b4).to_string();
    std::string b5s = std::bitset<8>(b5).to_string();
    std::string b6s = std::bitset<8>(b6).to_string();
    std::string b7s = std::bitset<8>(b7).to_string();
    std::string s_result = b0s + b1s + b2s + b3s + b4s + b5s + b6s + b7s;
    return std::stoull(s_result, nullptr, 2);
}

/**
 * Merge two bytes into a single 16-bit integer.
 *
 * @param initialByte Unsigned 8-bit integer
 * @param extraByte1 Unsigned 8-bit integer
 * @return Unsigned 16-bit integer containing thw merged bytes
 */
uint32_t mergeBytes16(const uint8_t& initialByte, const uint8_t& extraByte1)
{
    return (initialByte << 8) | extraByte1;
}

}
