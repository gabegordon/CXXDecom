#pragma once
#include <fstream>
#include <string>

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

    /**
     * Checks if a filestream is valid and open.
     * Prints error message and exits if invalid.
     *
     * @param stream Stream to check
     * @param name Name to print out for debugging purposes
     * @return N/A
     */
    static inline void checkFile(std::ifstream& stream, const std::string& name)
    {
        if (!stream || !stream.is_open())
        {
            std::cerr << "Failed to open " << name << " file." << std::endl;
            (void) system("pause");
            exit(0);
        }
    }
}
