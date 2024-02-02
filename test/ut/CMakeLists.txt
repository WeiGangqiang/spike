file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "*.c" "*.C" "*.cc" "*.CC" "*.cpp" "*.CPP" "*.c++")

add_executable(MemBuf_test ${SOURCES})

target_include_directories(MemBuf_test PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(MemBuf_test PUBLIC /usr/local/include)

target_link_libraries(MemBuf_test PUBLIC MemBuf gtest gtest_main)