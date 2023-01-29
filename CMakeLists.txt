cmake_minimum_required(VERSION 3.20)
project(hfw_localizer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GAME_PATH "D:\\SteamLibrary\\steamapps\\common\\Horizon Forbidden West Complete Edition")

add_definitions(-D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN -DWIN32_EXTRA_LEAN -DRTTI_STANDALONE)
add_subdirectory(libs)

add_library(hfw_localizer SHARED
        source/Decima/Core/RTTI.cpp
        source/Injector/Injector.cpp
        source/Util/Offsets.cpp
        source/Util/XUtil.cpp
        source/winhttp.c
        source/dllmain.cpp
)

target_include_directories(hfw_localizer PRIVATE source)
target_link_libraries(hfw_localizer PRIVATE detours nlohmann_json::nlohmann_json)
set_target_properties(hfw_localizer PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_custom_command(TARGET hfw_localizer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:hfw_localizer>
        "\"${GAME_PATH}\\winhttp.dll\""
)
