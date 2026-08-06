cmake_minimum_required(VERSION 3.21)

project(mwtl VERSION 0.1.0 LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "mwtl milestone 1 supports Windows only.")
endif()

if(NOT MSVC)
    message(FATAL_ERROR "mwtl milestone 1 supports the MSVC toolchain only (not MinGW or clang-cl).")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "mwtl milestone 1 supports x64 and ARM64 only; 32-bit targets are not supported.")
endif()

if(CMAKE_GENERATOR_PLATFORM AND NOT CMAKE_GENERATOR_PLATFORM MATCHES "^(x64|ARM64)$")
    message(FATAL_ERROR "mwtl milestone 1 supports only the x64 and ARM64 generator platforms.")
endif()

if(PROJECT_IS_TOP_LEVEL)
    option(MWTL_BUILD_EXAMPLES "Build mwtl examples" ON)
    option(MWTL_BUILD_TESTS "Build mwtl tests" ON)
else()
    option(MWTL_BUILD_EXAMPLES "Build mwtl examples" OFF)
    option(MWTL_BUILD_TESTS "Build mwtl tests" OFF)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)
mwtl_resolve_dependencies()

add_library(mwtl STATIC
    src/application.cpp
    src/detail/diagnostics.cpp
    src/module.cpp)
add_library(mwtl::mwtl ALIAS mwtl)

target_compile_features(mwtl PUBLIC cxx_std_17)
target_compile_definitions(mwtl
    PUBLIC
        UNICODE
        _UNICODE
        WINVER=0x0A00
        _WIN32_WINNT=0x0A00)
target_compile_options(mwtl
    PRIVATE
        /W4
        /permissive-
        /EHsc
        /utf-8)
target_include_directories(mwtl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(mwtl
    PUBLIC
        WTL::WTL
        WIL::WIL)

if(MWTL_BUILD_TESTS)
    target_compile_definitions(mwtl PRIVATE MWTL_TESTING)
endif()

if(MWTL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(MWTL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
