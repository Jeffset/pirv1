cmake_minimum_required(VERSION 3.9)
project(pirv1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()
add_compile_options(-O2)

add_executable(pirv1 main.cpp matrix.cc matrix.hpp)
target_link_libraries(pirv1 PRIVATE OpenMP::OpenMP_CXX)