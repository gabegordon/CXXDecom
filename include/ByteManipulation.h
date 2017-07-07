#pragma once
#include <iostream>
#include <cstdint>

namespace ByteManipulation {
    std::ostream &operator<<(std::ostream &os, char c);

    std::ostream &operator<<(std::ostream &os, signed char c);

    std::ostream &operator<<(std::ostream &os, unsigned char c);

    uint16_t swapEndian16(const uint16_t& val);

    uint32_t swapEndian32(const uint32_t& val);

    uint64_t swapEndian64(const uint64_t& val);

    uint32_t extract8(const uint8_t& val, const uint32_t& start, const uint32_t& len);

    uint32_t extract16(const uint16_t& val, const uint32_t& start, const uint32_t& len);

    uint32_t extract32(const uint32_t& val, const uint32_t& start, const uint32_t& len);

    uint64_t extract64(const uint64_t& val, const uint32_t& start, const uint32_t& len);

    int32_t extract8Signed(const uint8_t& val, const uint32_t& start, const uint32_t& len);

    int32_t extract16Signed(const uint16_t& val, const uint32_t& start, const uint32_t& len);

    int32_t extract32Signed(const uint32_t& val, const uint32_t& start, const uint32_t& len);

    int64_t extract64Signed(const uint64_t& val, const uint32_t& start, const uint32_t& len);

    uint32_t mergeBytes(const uint8_t& initialByte, const uint8_t& extraByte1, const uint8_t& extraByte2, const uint8_t& extraByte3, const uint32_t& num);

    uint64_t mergeBytes64(const uint8_t& b0, const uint8_t& b1, const uint8_t& b2, const uint8_t& b3, const uint8_t& b4, const uint8_t& b5, const uint8_t& b6, const uint8_t& b7);

    uint32_t mergeBytes16(const uint8_t& initialByte, const uint8_t& extraByte1);
}
