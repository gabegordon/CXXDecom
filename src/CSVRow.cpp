#include <sstream>
#include "CSVRow.h"

/**
 * Overrides array [] operator for accessing CSVRow elements.
 *
 * @param index Row index that we want
 * @return String contained at row index
 */
std::string const& CSVRow::operator[](std::size_t index) const
{
    return m_data.at(index);
}

/**
 * Get CSVRow data vector size.
 * 

 * @return Number of vector elements
 */
std::size_t CSVRow::size() const
{
    return m_data.size();
}


/**
 * Read a single row from CSV file. Splits on commas. Also handles commas that appear between double quotes.
 *
 * @param str Stream to read from
 * @return void. Output is pushed into vector
 */
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
