#include "main.h"
#include <iostream>

DWORD _;

typedef enum PatchType {
    Hex, Int, Byte, Float, Double
} PatchType;

PatchType GetPatchType(LPCSTR str)
{
    UINT length = strlen(str);

    if (length == 0)
        goto done;

    switch (tolower(str[0]))
    {
        case 'h':
            return Hex;
        case 'i':
            return Int;
        case 'f':
            return Float;
        case 'd':
            return Double;
        case 'b':
            return Byte;
    }

done:
    return Hex;
}

void Patch(LPVOID vOffset, LPVOID mem, UINT len)
{
    VirtualProtect(vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memcpy(vOffset, mem, len);
}

bool IsCharHexadecimal(char c)
{
    return (c >= '0' && c <= '9') || (tolower(c) >= 'a' && tolower(c) <= 'f');
}

void StringToHex(LPCSTR str, std::string &dest)
{
    int hexIndex = 0;
    char hexBytes[3] = { 0 };

    for (UINT i = 0; str[i] != NULL; ++i)
    {
        if (IsCharHexadecimal(str[i]))
            hexBytes[hexIndex++] = str[i];

        if (hexIndex == 2)
        {
            dest += (char) strtoul(hexBytes, NULL, 16);

            hexIndex = 0;
            ZeroMemory(hexBytes, 2);
        }
    }
}

HMODULE __stdcall LoadLibraryAHook(LPCSTR lpLibFileName) {
    HMODULE result = LoadLibraryA(lpLibFileName);

    if (result != NULL)
    {
        if (stricmp(lpLibFileName, "rpclocal.dll") == 0)
        {
            DWORD loadLibraryAddrRpc = (DWORD) result + 0xF210;
            SetLoadLibraryAHook(loadLibraryAddrRpc);
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

void Init(UINT onlyAllowedModule = NULL)
{
    INI_Reader reader;

    if (!reader.open("patches.ini"))
        return;

    while (reader.read_header())
    {
        if (!reader.is_header("Patch"))
            continue;

        UINT module = 0x400000;
        UINT offset = NULL;
        PatchType patchType = Hex;

        while (reader.read_value())
        {
            if (reader.is_value("module"))
            {
                module = (UINT) GetModuleHandleA(reader.get_value_string());
            }

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

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    // TODO: Support FLServer
    // TODO: Hook Server.dll load on client
    // IsMPServer common.dll might be useful
    // Call set value functions common.dll
    // Split up into multiple files
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        Init();

        DWORD loadLibraryAddrFl = 0x5B6F48;
        SetLoadLibraryAHook(loadLibraryAddrFl);
    }

    return TRUE;
}
