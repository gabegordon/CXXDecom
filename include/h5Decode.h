#pragma once

#include <string>

class h5Decode
{
  public:
  h5Decode(const std::string& directory) :
    m_directory(directory)
    {
        init();
    }

    virtual ~h5Decode() {}

  private:
    std::string m_directory;

    void init();
};
