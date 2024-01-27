#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

class Import INI_Reader
{
public:
    INI_Reader();
    ~INI_Reader();

    bool open(LPCSTR path, bool throwExceptionOnFail = false);
    bool read_header();
    bool is_header(LPCSTR name);
    bool read_value();
    bool is_value(LPCSTR name);
    int get_value_int(UINT index = 0);
    float get_value_float(UINT index = 0);
    LPCSTR get_value_string(UINT index = 0);
    void close();

private:
    BYTE data[0x1565];
};

bool Import IsMPServer();
