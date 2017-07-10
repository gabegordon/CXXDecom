#pragma once
#include <fstream>

namespace ReadFile
{
    template <typename T>
/**
 * Templated read function. Reads Type T from stream.
 *
 * @param buffer Data type to read into
 * @param in Stream to read from
 * @return N/A
 */
    static inline void read(T& buffer, std::ifstream& in)
    {
        in.read(reinterpret_cast<char *>(&buffer), sizeof(buffer));
    }
}
