#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Generic function definitions
typedef void (__fastcall GenericClassFunc)(PVOID thisptr);
typedef void (GenericFunc)();

struct FuncInfo
{
    LPCSTR moduleName;
    DWORD fileOffset;
};

struct ClassFuncInfo
{
    LPCSTR moduleName;
    DWORD fileOffset;
    DWORD thisPtr;
};

// This function is used to effectively re-set some internal values in the Freelancer binaries
// We use this because otherwise some patches may have no effect due to memory being read before FLPatch is loaded
// Thanks to this function, the patched values will actually be used
void SetInternalValues(bool isDebug);
