#include "debug.h"
#include "utils.h"
#include "Dacom.h"

void DumpIntPatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, int newValue)
{
    int oldValue;
    ReadBytes(&oldValue, vOffset, sizeof(int));

    FDUMP(SEV_NOTICE, "%sPatching: Module %s, File offset %X, Type %s, %ii -> %ii, Virtual address %X.",
        noticeLog, moduleName.c_str(), fileOffset, ToString(Int), oldValue, newValue, vOffset);
}

void DumpSBytePatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, char newValue)
{
    char oldValue;
    ReadBytes(&oldValue, vOffset, sizeof(char));

    FDUMP(SEV_NOTICE, "%sPatching: Module %s, File offset %X, Type %s, %ib -> %ib, Virtual address %X.",
        noticeLog, moduleName.c_str(), fileOffset, ToString(Byte), oldValue, newValue, vOffset);
}

void DumpFloatPatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, float newValue)
{
    float oldValue;
    ReadBytes(&oldValue, vOffset, sizeof(float));

    FDUMP(SEV_NOTICE, "%sPatching: Module %s, File offset %X, Type %s, %ff -> %ff, Virtual address %X.",
        noticeLog, moduleName.c_str(), fileOffset, ToString(Float), oldValue, newValue, vOffset);
}

void DumpDoublePatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, double newValue)
{
    double oldValue;
    ReadBytes(&oldValue, vOffset, sizeof(double));

    FDUMP(SEV_NOTICE, "%sPatching: Module %s, File offset %X, Type %s, %fd -> %fd, Virtual address %X.",
        noticeLog, moduleName.c_str(), fileOffset, ToString(Double), oldValue, newValue, vOffset);
}

void DumpHexPatch(const std::string &moduleName, DWORD fileOffset, LPVOID vOffset, const std::string &newValue)
{
    LPSTR oldValue = new char[newValue.size()];
    ReadBytes(oldValue, vOffset, newValue.size());

    LPSTR oldValueStr = BytesToHexString((LPCSTR) oldValue, newValue.size());
    LPSTR newValueStr = BytesToHexString(newValue.c_str(), newValue.size());

    FDUMP(SEV_NOTICE, "%sPatching: Module %s, File offset %X, Type %s, %s -> %s, Virtual address %X.",
        noticeLog, moduleName.c_str(), fileOffset, ToString(Hex), oldValueStr, newValueStr, vOffset);

    delete[] oldValue;
    delete[] oldValueStr;
    delete[] newValueStr;
}
