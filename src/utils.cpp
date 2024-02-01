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

void ReadBytes(LPVOID dest, LPVOID src, UINT len)
{
    static DWORD _;

    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &_);
    memcpy(dest, src, len);
}

void Nop(LPVOID vOffset, UINT len)
{
    if (len == 0)
        return;

    static DWORD _;

    VirtualProtect(vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memset(vOffset, 0x90, len);
}

void Hook(DWORD location, DWORD hookFunc, UINT instrLen)
{
    if (instrLen < 5)
        return;

    // Set the opcode for the call instruction
    static BYTE callOpcode = 0xE8;
    Patch((PVOID) location, &callOpcode, sizeof(BYTE));

    // Set and calculate the relative offset for the hook function
    DWORD relOffset = hookFunc - location - 5;
    Patch((PVOID) (location + 1), &relOffset, sizeof(DWORD));

    // Nop out excess bytes
    Nop((PVOID) (location + 5), instrLen - 5);
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

LPSTR BytesToHexString(LPCSTR bytes, UINT len)
{
    LPSTR result = new char[len * 3 + 1];

    for (UINT i = 0; i < len; ++i)
        sprintf(result + i * 3, "%02X ", (unsigned char) bytes[i]);

    result[len * 3 - 1] = '\0';

    return result;
}
