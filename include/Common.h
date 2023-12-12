#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

class Import INI_Reader
{
public:
    INI_Reader();
    ~INI_Reader();

    bool open(LPCSTR, bool = false);
    bool read_header();
    bool is_header(LPCSTR);
    bool read_value();
    bool is_value(LPCSTR);
    int get_value_int(UINT = 0);
    float get_value_float(UINT = 0);
    LPCSTR get_value_string(UINT = 0);
    void close();

private:
    BYTE data[0x1565];
};
