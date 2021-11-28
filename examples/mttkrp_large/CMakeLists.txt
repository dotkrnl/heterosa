include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    mttkrp-large-tapa
    INPUT         kernel.c
    SIMD_INFO     simd_info.json
    SA_SIZE       space_time[3] array_part[128,128,2] latency[16,8] simd[8,1]
    CONFIG        ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
)

add_dependencies(mttkrp-large-tapa heterosa_cc)
get_target_property(TAPA_PATH mttkrp-large-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL mttkrp-large-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(mttkrp-large)
    add_dependencies(mttkrp-large mttkrp-large-tapa)
    target_sources(mttkrp-large PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(mttkrp-large PUBLIC tapa::tapa frt::frt)
    add_test(NAME mttkrp-large-test COMMAND bash -c "TAPA_CONCURRENCY=16 ./mttkrp-large")
endif()