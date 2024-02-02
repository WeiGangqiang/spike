cmake_minimum_required (VERSION 3.4)
project (fotaTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-invalid-offsetof -g -std=c++14 -D_GLIBCXX_USE_CXX11_ABI=0")

include(cmake/gtest.cmake)
include(cmake/gbenchmark.cmake)

add_subdirectory(src)
add_subdirectory(test)


