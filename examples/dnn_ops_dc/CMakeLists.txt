include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    dnn-ops-dc-tapa
    INPUT     kernel.c
    SIMD_INFO simd_info.json
    SA_SIZE   space_time[4] array_part[4,4,4,3] latency[1,2,1] simd[1,2,1,1]
    CONFIG    ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
    --simd-touch-space
    --live-range-reordering
)

add_dependencies(dnn-ops-dc-tapa heterosa_cc)
get_target_property(TAPA_PATH dnn-ops-dc-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL dnn-ops-dc-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(dnn-ops-dc)
    add_dependencies(dnn-ops-dc dnn-ops-dc-tapa)
    target_sources(dnn-ops-dc PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(dnn-ops-dc PUBLIC tapa::tapa frt::frt)
    add_test(NAME dnn-ops-dc-test COMMAND dnn-ops-dc)
endif()