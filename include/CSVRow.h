#pragma once
#include <vector>
#include <string>
#include <fstream>

class CSVRow
{
  public:
  CSVRow() :
    m_data()
    {};
    virtual ~CSVRow() {};

    std::string const& operator[](const std::size_t index) const;

    std::size_t size() const;

    void readNextRow(std::istream& str);


  private:
    std::vector<std::string> m_data;
};
