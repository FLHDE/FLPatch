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

void StringToHex(LPCSTR str, std::string &dest);
