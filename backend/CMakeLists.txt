set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(LLVM_FIND_VERSION 6.0.0)
include(FetchContent)
include(ExternalProject)
include(../cmake/FindLLVM.cmake)
include(../cmake/FindClang.cmake)

find_library(gmp gmp)
if (NOT gmp)
    message(FATAL_ERROR "gmp not found")
endif()
find_library(ntl ntl)
if (NOT ntl)
    message(FATAL_ERROR "ntl not found")
endif()
find_library(pthread pthread)
if (NOT pthread)
    message(FATAL_ERROR "pthread not found")
endif()
if (NOT LLVM_FOUND)
    message(FATAL_ERROR "llvm not found")
endif()
if (NOT CLANG_FOUND)
    message(FATAL_ERROR "clang not found")
endif()

macro(ImportAutomake name additional_config)
FetchContent_GetProperties(${name})
FetchContent_Populate(${name})
ExternalProject_Add(
    ${name}
    PREFIX ${${name}_BINARY_DIR}
    SOURCE_DIR ${${name}_SOURCE_DIR}
    CONFIGURE_COMMAND
        bash -c "cd ${${name}_SOURCE_DIR} && \
            ([ -f ./get_submodules.sh ] && ./get_submodules.sh || true) && \
            ./autogen.sh" &&
        bash -c "${${name}_SOURCE_DIR}/configure \
            --srcdir=${${name}_SOURCE_DIR} \
            --prefix=${${name}_BINARY_DIR} \
            ${additional_config}"
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND make install
)
file(MAKE_DIRECTORY ${${name}_BINARY_DIR}/include)
endmacro()

FetchContent_Declare(
    libisl
    GIT_REPOSITORY "git://repo.or.cz/isl.git"
    GIT_TAG "master"
    GIT_SUBMODULES ""
    PATCH_COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/ast_type.h include/isl/
    COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/schedule_node.h include/isl/
    COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/schedule.h include/isl/
    COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/vec.h include/isl/
    COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/isl_schedule_tree.c .
    COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/isl_schedule_tree.h .
    COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/isl_schedule_node.c .
    COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/isl_schedule_band.c .
    COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/isl_schedule_band.h .
    COMMAND
    ln -sf ${CMAKE_CURRENT_LIST_DIR}/patches/isl_schedule.c .
)
ImportAutomake(libisl "")
add_library(isl STATIC IMPORTED GLOBAL)
add_dependencies(isl libisl)
set_target_properties(
    isl
    PROPERTIES
    IMPORTED_LOCATION ${libisl_BINARY_DIR}/lib/libisl.a
    IMPORTED_LINK_INTERFACE_LIBRARIES "gmp"
    INTERFACE_INCLUDE_DIRECTORIES ${libisl_BINARY_DIR}/include
)

FetchContent_Declare(
    libpet
    GIT_REPOSITORY "git://repo.or.cz/pet.git"
    GIT_TAG "master"
    GIT_SUBMODULES ""
)
ImportAutomake(libpet
    "--with-isl=system --with-isl-prefix=${libisl_BINARY_DIR}")
add_dependencies(libpet libisl)
add_library(pet STATIC IMPORTED GLOBAL)
add_dependencies(pet libpet)
set_target_properties(
    pet
    PROPERTIES
    IMPORTED_LOCATION ${libpet_BINARY_DIR}/lib/libpet.a
    IMPORTED_LINK_INTERFACE_LIBRARIES
        "isl;gmp;${CLANG_LIBS};${LLVM_LIBRARIES}"
    INTERFACE_INCLUDE_DIRECTORIES ${libpet_BINARY_DIR}/include
)

FetchContent_Declare(
    libbarvinok
    GIT_REPOSITORY "git://repo.or.cz/barvinok.git"
    GIT_TAG "master"
    GIT_SUBMODULES ""
)
ImportAutomake(libbarvinok "")
add_library(barvinok STATIC IMPORTED GLOBAL)
add_dependencies(barvinok libbarvinok)
set_target_properties(
    barvinok
    PROPERTIES
    IMPORTED_LOCATION ${libbarvinok_BINARY_DIR}/lib/libbarvinok.a
    IMPORTED_LINK_INTERFACE_LIBRARIES
        "isl;gmp;ntl;pthread;${libbarvinok_BINARY_DIR}/lib/libpolylibgmp.a"
    INTERFACE_INCLUDE_DIRECTORIES ${libbarvinok_BINARY_DIR}/include
)

FetchContent_Declare(
    cJSON
    GIT_REPOSITORY "https://github.com/DaveGamble/cJSON.git"
    GIT_TAG "master"
    GIT_SUBMODULES ""
)
FetchContent_GetProperties(cjson)
if(NOT cjson_POPULATED)
    FetchContent_Populate(cjson)
    SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Prefer static libraries")
    add_subdirectory(
        ${cjson_SOURCE_DIR} ${cjson_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_executable(heterosa_cc)
configure_file(version_config.h.in version_config.h)
target_sources(
    heterosa_cc
    PRIVATE
    cpu.c
    cpu.h
    grouping.c
    grouping.h
    schedule.c
    schedule.h
    ppcg_options.c
    ppcg_options.h
    ppcg.c
    ppcg.h
    print.c
    print.h
    util.c
    util.h
    version.c
    version_config.h.in
    main.cpp
    autosa_codegen.cpp
    autosa_comm.cpp
    autosa_common.cpp
    autosa_print.cpp
    autosa_schedule_tree.cpp
    autosa_trans.cpp
    autosa_utils.cpp
    autosa_xilinx_hls_c.cpp
)
target_include_directories(
    heterosa_cc
    PRIVATE
    "${cjson_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(
    heterosa_cc
    PRIVATE
    isl
    pet
    barvinok
    cjson
)

include(GNUInstallDirs)
install(
  TARGETS heterosa_cc
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)