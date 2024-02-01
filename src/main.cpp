#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "utils.h"
#include "internal.h"
#include "Common.h"
#include "Dacom.h"
#include "debug.h"

#define LOAD_LIBRARY_RPC_OFFSET 0xF20E
#define LOAD_LIBRARY_SERVER_OFFSET 0x63CA2
#define LOAD_LIBRARY_FL_ADDR 0x5B6F46

#define CAN_SKIP (onlyAllowedModule != NULL && onlyAllowedModule != module)

bool isDebug = false;

void ReadIsDebug()
{
    INI_Reader reader;

    if (!reader.open("patches.ini"))
        return;

    while (reader.read_header())
    {
        if (!reader.is_header("Options"))
            continue;

        while (reader.read_value())
        {
            if (reader.is_value("debug"))
                isDebug = reader.get_value_bool();
        }
    }

    reader.close();
}

void LoadPatches(UINT onlyAllowedModule = NULL)
{
    INI_Reader reader;

    if (!reader.open("patches.ini"))
    {
        if (isDebug)
            FDUMP(SEV_WARNING, "%sCannot open patches.ini.", warningLog);

        return;
    }

    while (reader.read_header())
    {
        if (!reader.is_header("Patch"))
            continue;

        std::string moduleName;
        DWORD module, offset = NULL;
        PatchType patchType = Hex;
        bool success = false; // Success means patched without error logged or skipped with error logged

        while (reader.read_value())
        {
            if (reader.is_value("module"))
            {
                moduleName = std::string(reader.get_value_string());
                module = (DWORD) GetModuleHandleA(moduleName.c_str());

                // Wtf?
                if (stricmp(moduleName.c_str(), "dalib.dll") == 0)
                    module += 0xC00;
            }

            if (reader.is_value("offset"))
                offset = (DWORD) strtoul(reader.get_value_string(), NULL, 16);

            if (reader.is_value("type"))
                patchType = GetPatchType(reader.get_value_string());

            if (reader.is_value("value"))
            {
                // If we specified that we only want to apply patches to a certain module, then don't patch in other modules
                if (CAN_SKIP)
                    continue;

                // If the module or the offset are null, do not patch
                if (module == NULL)
                {
                    if (isDebug)
                        FDUMP(SEV_WARNING, "%sModule %s could not be found for patch: File offset %X, Type %s. Not patching.", warningLog, moduleName.c_str(), offset, ToString(patchType));

                    success = true;
                    continue;
                }

                if (offset == NULL)
                {
                    if (isDebug)
                        FDUMP(SEV_ERROR, "%sNo offset specified for patch: Module %s, Type %s. Not patching.", errorLog, moduleName.c_str(), ToString(patchType));

                    success = true;
                    continue;
                }

                LPVOID vOffset = (LPVOID) (module + offset);

                // Apply the correct patch based on the type
                switch (patchType)
                {
                    case Int:
                    {
                        int intVal = reader.get_value_int();

                        if (isDebug)
                            DumpIntPatch(moduleName, offset, vOffset, intVal);

                        Patch(vOffset, &intVal, sizeof(int));
                        break;
                    }
                    case Byte:
                    {
                        char byteVal = (char) reader.get_value_int();

                        if (isDebug)
                            DumpSBytePatch(moduleName, offset, vOffset, byteVal);

                        Patch(vOffset, &byteVal, sizeof(char));
                        break;
                    }
                    case Float:
                    {
                        float floatVal = reader.get_value_float();

                        if (isDebug)
                            DumpFloatPatch(moduleName, offset, vOffset, floatVal);

                        Patch(vOffset, &floatVal, sizeof(float));
                        break;
                    }
                    case Double:
                    {
                        double doubleVal = (double) reader.get_value_float();

                        if (isDebug)
                            DumpDoublePatch(moduleName, offset, vOffset, doubleVal);

                        Patch(vOffset, &doubleVal, sizeof(double));
                        break;
                    }
                    case Hex:
                    {
                        std::string bytes;
                        StringToHex(reader.get_value_string(), bytes);

                        if (isDebug)
                            DumpHexPatch(moduleName, offset, vOffset, bytes);

                        Patch(vOffset, bytes.begin(), bytes.size());
                        break;
                    }
                }

                success = true;
            }
        }

        if (!success && isDebug && !CAN_SKIP)
            FDUMP(SEV_ERROR, "%sNo value specified for patch: Module %s, File offset %X, Type %s. Not patched.", errorLog, moduleName.c_str(), offset, ToString(patchType));
    }

    reader.close();
}

// This hook is used to apply patches in content.dll when the client or server loads it
HMODULE __stdcall LoadLibraryAHookServer(LPCSTR absLibFileName, LPCSTR lpLibFileName)
{
    bool contentModuleAlreadyLoaded = GetModuleHandleA("content.dll") != 0;

    HMODULE result = LoadLibraryA(lpLibFileName);

    if (!contentModuleAlreadyLoaded && stricmp(absLibFileName, "content.dll") == 0 && result != NULL)
    {
        if (isDebug)
            FDUMP(SEV_NOTICE, "%scontent.dll loaded; patching only in content.dll.", noticeLog);

        LoadPatches((UINT) result);
    }

    return result;
}

void SetLoadLibraryAHookServer()
{
    DWORD loadLibraryServerAddr = (DWORD) GetModuleHandleA("server.dll") + LOAD_LIBRARY_SERVER_OFFSET;

    BYTE pushEsi = 0x56;
    Patch((LPVOID) loadLibraryServerAddr, &pushEsi, sizeof(BYTE));

    // Set hook for loading content.dll
    Hook(loadLibraryServerAddr + 1, (DWORD) LoadLibraryAHookServer, 5);

    if (isDebug)
        FDUMP(SEV_NOTICE, "%sModule load hook set in server.dll; waiting for content.dll to load.", noticeLog);
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
            if (isDebug)
                FDUMP(SEV_NOTICE, "%srpclocal.dll loaded; waiting for server.dll to load.", noticeLog);

            // rpclocal.dll is responsible for loading server.dll on the client side, so we set a hook there
            DWORD loadLibraryRpcAddr = (DWORD) result + LOAD_LIBRARY_RPC_OFFSET;
            Hook(loadLibraryRpcAddr, (DWORD) LoadLibraryAHook, 6);
        }
        else if (stricmp(lpLibFileName, "server.dll") == 0)
        {
            if (isDebug)
                FDUMP(SEV_NOTICE, "%sserver.dll loaded; patching only in server.dll.", noticeLog);

            // This means server.dll has been loaded by rpclocal.dll
            // Now we want to apply all server.dll patches since those couldn't be applied before
            LoadPatches((UINT) result);

            // Set hook for loading content.dll
            SetLoadLibraryAHookServer();
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
        if (isDebug)
            FDUMP(SEV_NOTICE, "%sReading debug options.", noticeLog);

        ReadIsDebug();

        if (isDebug)
            FDUMP(SEV_NOTICE, "%sPatching all.", noticeLog);

        LoadPatches();

        if (isDebug)
            FDUMP(SEV_NOTICE, "%sCalling set internal value functions.", noticeLog);

        SetInternalValues(isDebug);


        if (IsMPServer())
        {
            // Set hook for loading content.dll
            SetLoadLibraryAHookServer();
        }
        else
        {
            // If the client is running (i.e. not the server), we want to set a hook so we can apply patches to server.dll as well since this one gets loaded later
            Hook(LOAD_LIBRARY_FL_ADDR, (DWORD) LoadLibraryAHook, 6);

            if (isDebug)
                FDUMP(SEV_NOTICE, "%sModule load hook set in Freelancer.exe; waiting for rpclocal.dll to load.", noticeLog);
        }
    }

    return TRUE;
}
