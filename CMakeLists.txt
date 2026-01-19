cmake_minimum_required(VERSION 3.30)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Ensure to pick up the default triplet from the environment if any. This helps
# driving the vcpkg triplet in the same way either when starting vcpkg directly,
# or when letting CMake start vcpkg at configure/generate time.
# Note: this logic must happen before PROJECT command.
if (DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "The vcpkg triplet")
endif()

project(benchmarks CXX)

if (NOT WIN32)
    add_compile_options(-Wall -Wextra -Wnon-virtual-dtor -pedantic -Werror -pipe -fvisibility=hidden)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-Wimplicit-fallthrough -Wmove)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(ENABLE_UNITY_BUILD "Enable unity build" ON)
set(CMAKE_UNITY_BUILD ${ENABLE_UNITY_BUILD})

include(CheckIPOSupported)
check_ipo_supported(
    RESULT CMAKE_INTERPROCEDURAL_OPTIMIZATION
    OUTPUT INTERPROCEDURAL_OPTIMIZATION_ERROR
    LANGUAGES CXX
)
if (NOT CMAKE_INTERPROCEDURAL_OPTIMIZATION)
    message(STATUS "IPO is not supported: ${INTERPROCEDURAL_OPTIMIZATION_ERROR}")
endif ()

add_subdirectory(src)
