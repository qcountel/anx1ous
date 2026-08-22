add_executable(anx1ousInjector main.cpp Download.cpp Download.h)

target_include_directories(anx1ousInjector PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_compile_definitions(anx1ousInjector PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE _UNICODE)

target_compile_options(anx1ousInjector PRIVATE /W4 /permissive- /EHsc /utf-8 /GR- /Gy /Gw)

target_link_options(anx1ousInjector PRIVATE
    $<$<CONFIG:Release>:/OPT:REF /OPT:ICF /INCREMENTAL:NO>)

target_link_libraries(anx1ousInjector PRIVATE advapi32 winhttp shell32 ole32)

set_target_properties(anx1ousInjector PROPERTIES
    OUTPUT_NAME "anx1ousInjector"
    VS_DPI_AWARE "PerMonitor")

target_link_options(anx1ousInjector PRIVATE "/MANIFESTUAC:level='requireAdministrator' uiAccess='false'")
