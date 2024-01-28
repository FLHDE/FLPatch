#include "utils.h"

PatchType GetPatchType(LPCSTR str)
{
    if (strlen(str) == 0)
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
    static DWORD _;

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
