cmake_minimum_required(VERSION 3.21)
project(mwtl_api_probe LANGUAGES CXX)

if(NOT MWTL_SOURCE_DIR)
    message(FATAL_ERROR "Set MWTL_SOURCE_DIR to the mwtl source directory")
endif()
add_subdirectory("${MWTL_SOURCE_DIR}" mwtl-source EXCLUDE_FROM_ALL)
add_executable(mwtl_api_probe WIN32 main.cpp ../../examples/example.manifest)
set_property(SOURCE ../../examples/example.manifest PROPERTY VS_TOOL_OVERRIDE "Manifest")
target_compile_features(mwtl_api_probe PRIVATE cxx_std_20)
target_compile_options(mwtl_api_probe PRIVATE /W4 /permissive- /EHsc /utf-8)
target_link_libraries(mwtl_api_probe PRIVATE mwtl::mwtl)

