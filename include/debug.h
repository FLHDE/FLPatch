#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

void DumpIntPatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, int newValue);

void DumpSBytePatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, char newValue);

void DumpFloatPatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, float newValue);

void DumpDoublePatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, double newValue);

void DumpHexPatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, const std::string &newValue);
