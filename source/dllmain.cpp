#include <Windows.h>

#include "Injector/Injector.h"

[[maybe_unused]] BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID *reserved) {
    (void) instance;
    (void) reserved;

    if (reason == DLL_PROCESS_ATTACH) {
        Injector::Attach();
    }

    if (reason == DLL_PROCESS_DETACH) {
        Injector::Detach();
    }

    return TRUE;
}
