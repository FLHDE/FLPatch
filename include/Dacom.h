#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

enum Severity
{
    SEV_ERROR   = 0x00100001,
    SEV_WARNING = 0x00100002,
    SEV_NOTICE  = 0x00100003
};

// The parameter pack contains optional values that are used to replace the format specifiers in formattedMessage
typedef int (*FDUMP_HANDLER) (Severity severity, LPCSTR formattedMessage, ...);

extern "C"
{
    Import FDUMP_HANDLER FDUMP;
}
