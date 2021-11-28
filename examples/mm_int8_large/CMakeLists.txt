include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    mm-int8-large-tapa
    INPUT         kernel.c
    SIMD_INFO     simd_info.json
    SA_SIZE       space_time[3] array_part[264,256,64] latency[11,32] simd[64]
    DATAPACK_SIZE A[32,32,64] B[32,32,64] C[32,32,64]
    CONFIG        ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
)

add_dependencies(mm-int8-large-tapa heterosa_cc)
get_target_property(TAPA_PATH mm-int8-large-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL mm-int8-large-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(mm-int8-large)
    add_dependencies(mm-int8-large mm-int8-large-tapa)
    target_sources(mm-int8-large PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(mm-int8-large PUBLIC tapa::tapa frt::frt)
    add_test(NAME mm-int8-large-test COMMAND mm-int8-large)
endif()