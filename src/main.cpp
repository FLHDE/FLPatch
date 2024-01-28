#include "main.h"
#include "utils.h"
#include "Common.h"

#define LOAD_LIBRARY_RPC_OFFSET 0xF210
#define LOAD_LIBRARY_FL_ADDR 0x5B6F48

void Init(UINT onlyAllowedModule = NULL)
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
                if (module == NULL || offset == NULL || (onlyAllowedModule != NULL && onlyAllowedModule != module))
                    continue;

                LPVOID vOffset = (LPVOID) (module + offset);

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

HMODULE __stdcall LoadLibraryAHook(LPCSTR lpLibFileName)
{
    HMODULE result = LoadLibraryA(lpLibFileName);

    if (result != NULL)
    {
        if (stricmp(lpLibFileName, "rpclocal.dll") == 0)
        {
            DWORD loadLibraryRpcAddr = (DWORD) result + LOAD_LIBRARY_RPC_OFFSET;
            SetLoadLibraryAHook(loadLibraryRpcAddr);
        }
        else if (stricmp(lpLibFileName, "server.dll") == 0)
        {
            Init((UINT) result);
        }
    }

    return result;
}

void SetLoadLibraryAHook(DWORD location)
{
    static DWORD hookPtr = (DWORD) LoadLibraryAHook;
    DWORD hookPtrRef = (DWORD) &hookPtr;

    Patch((PVOID) location, &hookPtrRef, sizeof(DWORD));
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    // TODO:
    // Call set value functions common.dll and Freelancer.exe
    // fl 1C8910: Single (float32) 20000 -> 40000. Increases the poly flipping distance, which allows jumpholes and other effects to be seen from further away.
    // common 13F48C: Single (float32) 10000 -> 50000. Increases the maximum docking initiation distance.
    // Add comments

    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        Init();

        if (!IsMPServer())
        {
            SetLoadLibraryAHook(LOAD_LIBRARY_FL_ADDR);
        }
    }

    return TRUE;
}
