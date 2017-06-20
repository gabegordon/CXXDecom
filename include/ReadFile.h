#pragma once
#include <fstream>

namespace ReadFile
{
    template <typename T>
    static inline void read(T& buffer, std::ifstream& in)
    {
        in.read(reinterpret_cast<char *>(&buffer), sizeof(buffer));
    }
}
