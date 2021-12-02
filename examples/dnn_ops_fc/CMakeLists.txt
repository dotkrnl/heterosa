include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    dnn-ops-fc-tapa
    INPUT     kernel.c
    SIMD_INFO simd_info.json
    SA_SIZE   space_time[2] array_part[8,4] latency[4] simd[2]
    CONFIG    ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
    --simd-touch-space --local-reduce --reduce-op=+
)

add_dependencies(dnn-ops-fc-tapa heterosa_cc)
get_target_property(TAPA_PATH dnn-ops-fc-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL dnn-ops-fc-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(dnn-ops-fc)
    add_dependencies(dnn-ops-fc dnn-ops-fc-tapa)
    target_sources(dnn-ops-fc PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(dnn-ops-fc PUBLIC tapa::tapa frt::frt)
    add_test(NAME dnn-ops-fc-test COMMAND dnn-ops-fc)
endif()