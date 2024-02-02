file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "*.c" "*.C" "*.cc" "*.CC" "*.cpp" "*.CPP" "*.c++")

add_library(MemBuf STATIC ${SOURCES})

target_include_directories(MemBuf
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        )

target_link_libraries(MemBuf PUBLIC pthread)
target_compile_options(MemBuf PUBLIC -fPIC)