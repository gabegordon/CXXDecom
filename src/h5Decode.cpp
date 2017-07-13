#include <iostream>
#include "h5Decode.h"
#include "getFiles.h"

void h5Decode::init()
{
    auto files = getFiles::h5InFolder(m_directory);
    if(files.size() == 0)
    {
        std::cout << "No .h5 files found" << std::endl;
        exit(0);
    }
    for(const auto& i : files)
    {
        std::cout << i << std::endl;
        
    }
}
