cmake_minimum_required(VERSION 3.21)

project(mwtl VERSION 0.5.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(NOT WIN32)
    message(FATAL_ERROR "mwtl supports Windows only.")
endif()

if(NOT MSVC)
    message(FATAL_ERROR "mwtl supports the MSVC toolchain only (not MinGW or clang-cl).")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "mwtl supports x64 only; 32-bit targets are not supported.")
endif()

if(CMAKE_GENERATOR_PLATFORM AND NOT CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
    message(FATAL_ERROR "mwtl currently supports only the x64 generator platform.")
endif()

if(CMAKE_CXX_COMPILER_ARCHITECTURE_ID AND
   NOT CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "x64")
    message(FATAL_ERROR
        "mwtl currently supports only x64; compiler architecture is ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}.")
endif()

if(PROJECT_IS_TOP_LEVEL)
    option(MWTL_BUILD_EXAMPLES "Build mwtl examples" ON)
    option(MWTL_BUILD_TESTS "Build mwtl tests" ON)
    option(MWTL_WARNINGS_AS_ERRORS "Treat mwtl warnings as errors" ON)
else()
    option(MWTL_BUILD_EXAMPLES "Build mwtl examples" OFF)
    option(MWTL_BUILD_TESTS "Build mwtl tests" OFF)
    option(MWTL_WARNINGS_AS_ERRORS "Treat mwtl warnings as errors" OFF)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)
mwtl_resolve_dependencies()

add_library(mwtl STATIC
    src/application.cpp
    src/command_controls.cpp
    src/control_resources.cpp
    src/controls.cpp
    src/input_controls.cpp
    src/layout.cpp
    src/navigation_controls.cpp
    src/dpi.cpp
    src/desktop.cpp
    src/detail/diagnostics.cpp
    src/message_pump.cpp
    src/module.cpp
    src/timer.cpp
    src/window_options.cpp)
add_library(mwtl::mwtl ALIAS mwtl)

get_target_property(mwtl_wtl_include_dirs WTL::WTL INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(mwtl_wil_include_dirs WIL::WIL INTERFACE_INCLUDE_DIRECTORIES)

target_compile_features(mwtl PUBLIC cxx_std_20)
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
if(MWTL_WARNINGS_AS_ERRORS)
    target_compile_options(mwtl PRIVATE /WX)
endif()
target_include_directories(mwtl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${mwtl_wtl_include_dirs}>
        $<BUILD_INTERFACE:${mwtl_wil_include_dirs}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(mwtl
    PRIVATE
        comctl32
        comdlg32
        advapi32
        shell32
        ole32)

install(TARGETS mwtl
    EXPORT mwtlTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES
    LICENSE
    THIRD_PARTY_NOTICES.md
    README.md
    CHANGELOG.md
    SECURITY.md
    docs/api-0.5.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/mwtl)
install(FILES cmake/Dependencies.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mwtl)

configure_package_config_file(
    cmake/mwtlConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/mwtlConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mwtl)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/mwtlConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/mwtlConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/mwtlConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mwtl)
install(EXPORT mwtlTargets
    FILE mwtlTargets.cmake
    NAMESPACE mwtl::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mwtl)

set(CPACK_PACKAGE_NAME "mwtl")
set(CPACK_PACKAGE_VENDOR "mwtl contributors")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_FILE_NAME "mwtl-${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_GENERATOR "ZIP")
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "mwtl-${PROJECT_VERSION}-source")
set(CPACK_SOURCE_IGNORE_FILES "/[.]git/;/build/;/[.]tmp/;~$")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
include(CPack)

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
