#include "internal.h"

LPCSTR flModule = "freelancer.exe";
LPCSTR commonModule = "common.dll";

void SetInternalValues()
{
    int i;

    const FuncInfo funcs[] =
    {
        { flModule, 0x10100 }, // Poly flipping distance #1
        { flModule, 0x10120 }, // Poly flipping distance #2
        { flModule, 0x11B2D0 }, // Poly flipping distance #3
        { commonModule, 0x62E40 }, // Maximum docking initiation distance
    };

    const ClassFuncInfo classFuncs[] =
    {
        { flModule, 0x1628E0, 0x67ADC4 } // Filter out incompatible builds on server by default
    };

    for (i = 0; i < sizeof(funcs) / sizeof(FuncInfo); ++i)
    {
        DWORD module = (DWORD) GetModuleHandleA(funcs[i].moduleName);

        if (!module)
            continue;

        GenericFunc* func = (GenericFunc*) (module + funcs[i].fileOffset);
        (func)();
    }

    for (i = 0; i < sizeof(classFuncs) / sizeof(ClassFuncInfo); ++i)
    {
        DWORD module = (DWORD) GetModuleHandleA(classFuncs[i].moduleName);

        if (!module)
            continue;

        GenericClassFunc* classFunc = (GenericClassFunc*) (module + classFuncs[i].fileOffset);

        (classFunc)((PVOID) classFuncs[i].thisPtr, NULL);
    }
}
