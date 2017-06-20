#pragma once
#ifdef _WIN64
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

#include <iostream>
#include <iomanip>
#include <cstring>

#define TOTAL_PERCENTAGE 100.0
#define CHARACTER_WIDTH_PERCENTAGE 4

class ProgressBar {

  public:

  ProgressBar(uint64_t n_, const char* description_ = "", std::ostream& out_ = std::cerr) :
    n(n_),
    desc_width(0),
    frequency_update(n_),
    description(description_)
    {
        unit_bar = "=";
        unit_space = " ";
        desc_width = std::strlen(description);
        out = &out_;
    }

    void SetFrequencyUpdate(uint64_t frequency_update_);
    void SetStyle(const char* unit_bar_, const char* unit_space_);

    void Progressed(uint64_t idx_);

  private:

    uint64_t n;
    uint64_t desc_width;
    uint64_t frequency_update;
    std::ostream* out;

    const char *description;
    const char *unit_bar;
    const char *unit_space;

    void ClearBarField();
    int GetConsoleWidth();
    int GetBarLength();

};
