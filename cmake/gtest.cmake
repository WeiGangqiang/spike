if (HAVE_GTEST)
    return()
endif()

include(ExternalProject)
cmake_policy(SET CMP0135 OLD)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/output)
set(REQ_URL "https://gitee.com/mirrors/googletest/repository/archive/release-1.8.1.tar.gz")

set (gtest_CXXFLAGS "-D_GLIBCXX_USE_CXX11_ABI=0 -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-all")
set (gtest_CFLAGS "-D_GLIBCXX_USE_CXX11_ABI=0 -D_FORTIFY_SOURCE=2 -O2 -fstack-protector-all")
ExternalProject_Add(gtest_build
        URL ${REQ_URL}
        TLS_VERIFY OFF
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_CXX_FLAGS=${gtest_CXXFLAGS} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/gtest <SOURCE_DIR>
        -DBUILD_TESTING=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBUILD_SHARED_LIBS=ON -DCMAKE_MACOSX_RPATH=TRUE
        BUILD_COMMAND $(MAKE)
        INSTALL_COMMAND $(MAKE) install
        EXCLUDE_FROM_ALL TRUE
        )

ExternalProject_Get_Property(gtest_build SOURCE_DIR)
set(GTEST_INCLUDE ${SOURCE_DIR}/googletest/include)

set(GTEST_PKG_DIR ${CMAKE_INSTALL_PREFIX}/gtest)

file(MAKE_DIRECTORY ${GTEST_PKG_DIR}/include)

add_library(gtest SHARED IMPORTED)

set_target_properties(gtest PROPERTIES
        IMPORTED_LOCATION ${GTEST_PKG_DIR}/lib/libgtest.dylib
        )

add_library(gtest_main SHARED IMPORTED)

set_target_properties(gtest_main PROPERTIES
        IMPORTED_LOCATION ${GTEST_PKG_DIR}/lib/libgtest_main.dylib
        )

target_include_directories(gtest INTERFACE ${GTEST_PKG_DIR}/include)
target_include_directories(gtest_main INTERFACE ${GTEST_PKG_DIR}/include)


add_library(gmock SHARED IMPORTED)

set_target_properties(gmock PROPERTIES
        IMPORTED_LOCATION ${GTEST_PKG_DIR}/lib/libgmock.so
        )

add_library(gmock_main SHARED IMPORTED)

set_target_properties(gmock_main PROPERTIES
        IMPORTED_LOCATION ${GTEST_PKG_DIR}/lib/libgmock_main.so
        )

target_include_directories(gmock INTERFACE ${GTEST_PKG_DIR}/include)
target_include_directories(gmock_main INTERFACE ${GTEST_PKG_DIR}/include)


set(INSTALL_BASE_DIR "")
set(INSTALL_LIBRARY_DIR lib)

install(FILES ${GTEST_PKG_DIR}/lib/libgtest.so ${GTEST_PKG_DIR}/lib/libgtest_main.so ${GTEST_PKG_DIR}/lib/libgmock.so ${GTEST_PKG_DIR}/lib/libgmock_main.so OPTIONAL
        DESTINATION ${INSTALL_LIBRARY_DIR})

install(DIRECTORY ${GTEST_INCLUDE} DESTINATION ${GTEST_PKG_DIR}/include)

add_dependencies(gtest gtest_build)

set(HAVE_GTEST TRUE)
