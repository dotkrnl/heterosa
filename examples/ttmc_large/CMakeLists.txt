include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    tmmc-large-tapa
    INPUT         kernel.c
    SIMD_INFO     simd_info.json
    SA_SIZE       space_time[4] array_part[16,64,16,32] latency[1,8,8] simd[1,1,8]
    CONFIG        ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
)

add_dependencies(tmmc-large-tapa heterosa_cc)
get_target_property(TAPA_PATH tmmc-large-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL tmmc-large-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(tmmc-large)
    add_dependencies(tmmc-large tmmc-large-tapa)
    target_sources(tmmc-large PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(tmmc-large PUBLIC tapa::tapa frt::frt)
    add_test(NAME tmmc-large-test COMMAND bash -c "TAPA_CONCURRENCY=16 ./tmmc-large")
endif()