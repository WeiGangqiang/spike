file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "*.c" "*.C" "*.cc" "*.CC" "*.cpp" "*.CPP" "*.c++")

add_executable(MemBuf_benchmark ${SOURCES})

target_include_directories(MemBuf_benchmark PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(MemBuf_benchmark PUBLIC /usr/local/include)

target_link_libraries(MemBuf_benchmark PUBLIC MemBuf benchmark benchmark_main)