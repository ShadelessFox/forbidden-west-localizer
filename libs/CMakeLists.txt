add_library(detours STATIC
        detours/src/disolia64.cpp
        detours/src/disolx64.cpp
        detours/src/detours.cpp
        detours/src/disolx86.cpp
        detours/src/disolarm.cpp
        detours/src/creatwth.cpp
        detours/src/disolarm64.cpp
        detours/src/image.cpp
        detours/src/disasm.cpp
        detours/src/modules.cpp
)
target_include_directories(detours PUBLIC detours/src)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(json)

add_library(quickdllproxy INTERFACE)
target_include_directories(quickdllproxy INTERFACE quickdllproxy/include)
