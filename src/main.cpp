#include "Common.h"
#include <iostream>

DWORD _;

typedef enum PatchType {
    Hex, Int, Byte, Float, Double
} PatchType;

PatchType GetPatchType(LPCSTR str) {
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

void Init()
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
                module = (UINT) GetModuleHandleA(reader.get_value_string());

            if (reader.is_value("offset"))
                offset = (UINT) strtoul(reader.get_value_string(), NULL, 16);

            if (reader.is_value("type"))
                patchType = GetPatchType(reader.get_value_string());

            if (reader.is_value("value"))
            {
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
                        LPCSTR strVal = reader.get_value_string();
                        std::string bytes;

                        StringToHex(strVal, bytes);

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

    if (fdwReason == DLL_PROCESS_ATTACH)
        Init();

    return TRUE;
}
