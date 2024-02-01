#include "internal.h"
#include "Dacom.h"

LPCSTR flModule = "freelancer.exe";
LPCSTR commonModule = "common.dll";

void SetInternalValues(bool isDebug)
{
    int i;

    // TODO: Read funcs and classFuncs from ini file?
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
        {
            if (isDebug)
                FDUMP(SEV_WARNING, "FLPatch.dll WARNING: Cannot find module %s for generic set value function: File offset %X. Not calling.",
                    funcs[i].moduleName, funcs[i].fileOffset);

            continue;
        }

        LPVOID vOffset = (LPVOID) (module + funcs[i].fileOffset);

        if (isDebug)
            FDUMP(SEV_NOTICE, "FLPatch.dll NOTICE: Calling generic set value function: Module %s, File offset %X, Virtual address %X.",
                funcs[i].moduleName, funcs[i].fileOffset, vOffset);

        GenericFunc* func = (GenericFunc*) vOffset;
        (func)();
    }

    for (i = 0; i < sizeof(classFuncs) / sizeof(ClassFuncInfo); ++i)
    {
        DWORD module = (DWORD) GetModuleHandleA(classFuncs[i].moduleName);

        if (!module)
        {
            if (isDebug)
                FDUMP(SEV_WARNING, "FLPatch.dll WARNING: Cannot find module %s for generic CLASS set value function: File offset %X, thisptr %X. Not calling.",
                    classFuncs[i].moduleName, classFuncs[i].fileOffset, classFuncs[i].thisPtr);

            continue;
        }

        LPVOID vOffset = (LPVOID) (module + classFuncs[i].fileOffset);

        if (isDebug)
            FDUMP(SEV_NOTICE, "FLPatch.dll NOTICE: Calling generic CLASS set value function: Module %s, File offset %X, thisptr %X, Virtual address %X.",
                classFuncs[i].moduleName, classFuncs[i].fileOffset, classFuncs[i].thisPtr, vOffset);

        GenericClassFunc* classFunc = (GenericClassFunc*) vOffset;
        (classFunc)((PVOID) classFuncs[i].thisPtr, NULL);
    }
}
