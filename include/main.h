#include "Common.h"

HMODULE __stdcall LoadLibraryAHook(LPCSTR lpLibFileName);

void SetLoadLibraryAHook(DWORD location);

void Init(UINT onlyAllowedModule);
