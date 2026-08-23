cmake_minimum_required(VERSION 3.21)
project(anx1ous LANGUAGES CXX VERSION 0.1.0)

if(NOT MSVC)
    message(FATAL_ERROR "anx1ous must be built with MSVC (ABI compatibility with the game binary).")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "anx1ous must be built for x64. Configure with -A x64.")
endif()

option(ANX1OUS_OBFUSCATE "Obfuscate cold code with an OLLVM clang-cl toolchain (Arkari)" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG        v1.3.4
    GIT_SHALLOW    ON)
FetchContent_MakeAvailable(minhook)

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    ON)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(nlohmann_json)

file(GLOB_RECURSE ANX1OUS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.h)

add_library(anx1ous SHARED ${ANX1OUS_SOURCES} ${CMAKE_SOURCE_DIR}/src/Assets/Resources.rc)

source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX src FILES ${ANX1OUS_SOURCES})

target_include_directories(anx1ous PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(anx1ous PRIVATE
    minhook
    nlohmann_json::nlohmann_json
    user32 shell32 ole32
    winmm                      # PlaySound, for the notification chime
    # the Direct2D overlay: dxguid for effect CLSIDs, d3dcompiler for the
    # composite shaders that blend our texture over the game's frame
    d2d1 dwrite d3d11 dxgi dxguid d3dcompiler
    windowscodecs)   # WIC, to decode the embedded PNGs

target_compile_definitions(anx1ous PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    UNICODE _UNICODE
    ANX1OUS_VERSION="${PROJECT_VERSION}"
    $<$<CONFIG:Debug>:ANX1OUS_LOGGING>)

# 4100: unreferenced parameter (event handlers ignore their event)
# 4201: nameless struct/union
# 4624: deleted destructor - deliberate on the SDK wrappers, which are never
#       constructed or destroyed by us, only reinterpret_cast from game pointers
target_compile_options(anx1ous PRIVATE
    /W4 /wd4100 /wd4201 /wd4624 /permissive- /EHsc /Zc:preprocessor /utf-8
    /GR- /Gy /Gw)
target_link_options(anx1ous PRIVATE
    $<$<CONFIG:Debug>:/DEBUG>
    $<$<CONFIG:Release>:/OPT:REF /OPT:ICF /OPT:LBR /INCREMENTAL:NO>)

set_target_properties(anx1ous PROPERTIES
    OUTPUT_NAME "anx1ous"
    PREFIX ""
    SUFFIX ".dll")

if(ANX1OUS_OBFUSCATE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR
            "ANX1OUS_OBFUSCATE needs a Clang/clang-cl toolchain (e.g. Arkari); "
            "current compiler is ${CMAKE_CXX_COMPILER_ID}. Configure with the 'obf' preset.")
    endif()

    set(ANX1OUS_OBF_FLAGS
        -mllvm -irobf-fla     # control flow flattening
        -mllvm -irobf-cse     # constant string encryption
        -mllvm -irobf-cie     # constant integer encryption
        -mllvm -irobf-cfe)    # constant float encryption

    set(ANX1OUS_HOT_REGEX "/src/Render/|/src/Hooks/GameHooks\\.cpp$|/src/Hooks/StabilityHooks\\.cpp$")

    foreach(src IN LISTS ANX1OUS_SOURCES)
        if(src MATCHES "\\.cpp$" AND NOT src MATCHES "${ANX1OUS_HOT_REGEX}")
            set_source_files_properties(${src} PROPERTIES COMPILE_OPTIONS "${ANX1OUS_OBF_FLAGS}")
        endif()
    endforeach()

    message(STATUS "anx1ous: obfuscation ON (render + GameHooks kept clean for FPS)")
endif()
