#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "utils.h"
#include "internal.h"
#include "Common.h"

#define LOAD_LIBRARY_RPC_OFFSET 0xF20E
#define LOAD_LIBRARY_FL_ADDR 0x5B6F46

void LoadPatches(UINT onlyAllowedModule = NULL)
{
    INI_Reader reader;

    if (!reader.open("patches.ini"))
        return;

    while (reader.read_header())
    {
        if (!reader.is_header("Patch"))
            continue;

        UINT module, offset = NULL;
        PatchType patchType = Hex;

        while (reader.read_value())
        {
            if (reader.is_value("module"))
                module = (UINT) GetModuleHandleA(reader.get_value_string());

            if (reader.is_value("offset"))
                offset = (UINT) strtoul(reader.get_value_string(), NULL, 16);

            if (reader.is_value("type"))
                patchType = GetPatchType(reader.get_value_string());

            if (reader.is_value("value"))
            {
                // If the module or the offset are null, do not patch
                // If we specified that we only want to apply patches to a certain module, then don't patch in other modules either
                if (module == NULL || offset == NULL || (onlyAllowedModule != NULL && onlyAllowedModule != module))
                    continue;

                LPVOID vOffset = (LPVOID) (module + offset);

                // Apply the correct patch based on the type
                switch (patchType)
                {
                    case Int:
                    {
                        int intVal = reader.get_value_int();
                        Patch(vOffset, &intVal, sizeof(int));
                        break;
                    }
                    case Byte:
                    {
                        char byteVal = (char) reader.get_value_int();
                        Patch(vOffset, &byteVal, sizeof(char));
                        break;
                    }
                    case Float:
                    {
                        float floatVal = reader.get_value_float();
                        Patch(vOffset, &floatVal, sizeof(float));
                        break;
                    }
                    case Double:
                    {
                        double doubleVal = (double) reader.get_value_float();
                        Patch(vOffset, &doubleVal, sizeof(double));
                        break;
                    }
                    case Hex:
                    {
                        std::string bytes;
                        StringToHex(reader.get_value_string(), bytes);

                        Patch(vOffset, bytes.begin(), bytes.size());
                        break;
                    }
                }
            }
        }
    }

    reader.close();
}

// This hook is used to apply patches in server.dll when the client loads it
// First we check when rpclocal.dll gets loaded by Freelancer.exe, then we place a hook in that dll. Then it should load server.dll later
HMODULE __stdcall LoadLibraryAHook(LPCSTR lpLibFileName)
{
    HMODULE result = LoadLibraryA(lpLibFileName);

    if (result != NULL)
    {
        if (stricmp(lpLibFileName, "rpclocal.dll") == 0)
        {
            // rpclocal.dll is responsible for loading server.dll on the client side, so we set a hook there
            DWORD loadLibraryRpcAddr = (DWORD) result + LOAD_LIBRARY_RPC_OFFSET;
            Hook(loadLibraryRpcAddr, (DWORD) LoadLibraryAHook, 6);
        }
        else if (stricmp(lpLibFileName, "server.dll") == 0)
        {
            // This means server.dll has been loaded by rpclocal.dll
            // Now we want to apply all server.dll patches since those couldn't be applied before
            LoadPatches((UINT) result);
        }
    }

    return result;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        LoadPatches();
        SetInternalValues();

        // If the client is running, we want to set a hook so we can apply patches to server.dll as well since this one gets loaded later
        if (!IsMPServer())
            Hook(LOAD_LIBRARY_FL_ADDR, (DWORD) LoadLibraryAHook, 6);
    }

    return TRUE;
}
