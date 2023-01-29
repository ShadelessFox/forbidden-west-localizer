#include <Windows.h>
#include <cstdio>

#include "Injector/Injector.h"

[[maybe_unused]] BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID *reserved) {
    (void) instance;
    (void) reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        AllocConsole();
        AttachConsole(ATTACH_PARENT_PROCESS);
        freopen("CON", "w", stdout);

        Injector::Attach();
    }

    if (reason == DLL_PROCESS_DETACH) {
        FreeConsole();

        Injector::Detach();
    }

    return TRUE;
}
