include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    tmm-large-tapa
    INPUT         kernel.c
    SIMD_INFO     simd_info.json
    SA_SIZE       space_time[5] array_part[20,256,4,128] latency[2,32,2] simd[8]
    CONFIG        ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
)

add_dependencies(tmm-large-tapa heterosa_cc)
get_target_property(TAPA_PATH tmm-large-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL tmm-large-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(tmm-large)
    add_dependencies(tmm-large tmm-large-tapa)
    target_sources(tmm-large PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(tmm-large PUBLIC tapa::tapa frt::frt)
    add_test(NAME tmm-large-test COMMAND bash -c "TAPA_CONCURRENCY=16 ./tmm-large")
endif()