include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    mm-float-large-tapa
    INPUT     kernel.c
    SIMD_INFO simd_info.json
    SA_SIZE   space_time[3] array_part[260,256,512] latency[20,16] simd[8]
    CONFIG    ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
)

add_dependencies(mm-float-large-tapa heterosa_cc)
get_target_property(TAPA_PATH mm-float-large-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL mm-float-large-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(mm-float-large)
    add_dependencies(mm-float-large mm-float-large-tapa)
    target_sources(mm-float-large PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(mm-float-large PUBLIC tapa::tapa frt::frt)
    add_test(NAME mm-float-large-test COMMAND bash -c "TAPA_CONCURRENCY=16 ./mm-float-large")
endif()