cmake_minimum_required(VERSION 3.10)

project(heterosa VERSION 0.0)

set(CPACK_PACKAGE_NAME heterosa)
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 20211125.1)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AutoSA fork targeting TAPA")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

add_subdirectory(backend)
enable_testing()
add_subdirectory(examples)

list(
    APPEND
    CPACK_DEBIAN_PACKAGE_DEPENDS
    "autoconf"
    "automake"
    "libtool"
    "pkg-config"
    "libclang-dev"
    "libgmp3-dev"
    "libisl-dev"
    "libntl-dev"
    "libyaml-dev"
    "llvm-dev"
)
list(SORT CPACK_DEBIAN_PACKAGE_DEPENDS)
list(JOIN CPACK_DEBIAN_PACKAGE_DEPENDS ", " CPACK_DEBIAN_PACKAGE_DEPENDS)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jason Lau")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_GENERATOR
    TGZ DEB
    CACHE STRING "Enable selected CPack generators")
include(CPack)
