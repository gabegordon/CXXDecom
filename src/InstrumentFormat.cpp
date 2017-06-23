#include <fstream>
#include <iostream>
#include "InstrumentFormat.h"


namespace InstrumentFormat
{
void InstrumentFormat::formatATMS()
{
    std::ifstream m_infile;
    m_infile.open("output/ATMS_528.txt", std::ios::in);
    if (!m_infile || !m_infile.is_open())
    {
        return;
    }
    std::cout << "HERE";
}
}
