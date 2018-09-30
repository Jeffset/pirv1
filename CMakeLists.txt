cmake_minimum_required(VERSION 3.9)
project(pirv1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)

add_executable(pirv1 main.cpp matrix.cc matrix.hpp)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
else()
    add_compile_options(-O3)
endif()
# target_link_libraries(pirv1 PRIVATE OpenMP::OpenMP_CXX)