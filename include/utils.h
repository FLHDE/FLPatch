#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

typedef enum PatchType
{
    Hex, Int, Byte, Float, Double
} PatchType;

PatchType GetPatchType(LPCSTR str);

void Patch(LPVOID vOffset, LPVOID mem, UINT len);

void Hook(DWORD location, DWORD hookFunc, UINT instrLen);

void StringToHex(LPCSTR str, std::string &dest);

inline LPCSTR ToString(PatchType pt)
{
    switch (pt)
    {
        case Hex:    return "Hex";
        case Int:    return "Int";
        case Float:  return "Float";
        case Double: return "Double";
        case Byte:   return "Byte";
    }

    return "Hex";
}
