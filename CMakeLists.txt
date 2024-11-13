cmake_minimum_required(VERSION 3.21)
project(DirtySocks VERSION 0.1.0 LANGUAGES CXX)

option(DS_MSVC_UTF8 "Use /utf-8 for MSVC" TRUE)
option(DS_WIN32_UNICODE "Define `_UNICODE` & `UNICODE` for MSVC" FALSE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

add_library(DirtySocks)
add_subdirectory(src)

target_compile_options(DirtySocks PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /Zc:preprocessor $<IF:$<BOOL:${DS_MSVC_UTF8}>,/utf-8,/source-charset:utf-8>>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

if(WIN32)
    target_link_libraries(DirtySocks PRIVATE ws2_32)
endif()
if(DS_WIN32_UNICODE)
    target_compile_definitions(DirtySocks PUBLIC _UNICODE UNICODE)
endif()

target_include_directories(DirtySocks PUBLIC include PRIVATE src)
