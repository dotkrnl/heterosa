include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    lu-tapa
    INPUT     kernel.c
    SIMD_INFO simd_info.json
    SA_SIZE   space_time[3] array_part[-1,-1,-1] latency[]
    CONFIG    ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
    --use-cplusplus-template
    --double-buffer-style=1 --no-reschedule
    --int-io-dir=1 --no-data-pack
    --live-range-reordering
)

add_dependencies(lu-tapa heterosa_cc)
get_target_property(TAPA_PATH lu-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL lu-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(lu)
    add_dependencies(lu lu-tapa)
    target_sources(lu PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(lu PUBLIC tapa::tapa frt::frt)
    add_test(NAME lu-test COMMAND lu)
endif()