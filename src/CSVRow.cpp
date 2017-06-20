#include <sstream>
#include "CSVRow.h"

std::string const& CSVRow::operator[](std::size_t index) const
{
    return m_data.at(index);
}

std::size_t CSVRow::size() const
{
    return m_data.size();
}

void CSVRow::readNextRow(std::istream& str)
{
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    m_data.clear();

    while (std::getline(lineStream, line)) {
        const char *mystart = line.c_str();
        bool instring{ false };
        for (const char* p = mystart; *p; p++) {
            if (*p == '"')
                instring = !instring;
            else if (*p == ',' && !instring) {
                m_data.push_back(std::string(mystart, p - mystart));
                mystart = p + 1;
            }
        }
        m_data.push_back(std::string(mystart));
    }
}
