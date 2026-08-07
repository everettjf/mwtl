cmake_minimum_required(VERSION 3.21)
project(mwtl_package_consumer LANGUAGES CXX)

find_package(mwtl 0.3 CONFIG REQUIRED)

add_executable(mwtl_package_consumer main.cpp)
target_link_libraries(mwtl_package_consumer PRIVATE mwtl::mwtl)
target_compile_features(mwtl_package_consumer PRIVATE cxx_std_20)
