include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    mm-int8-tapa
    INPUT     kernel.c
    SIMD_INFO simd_info.json
    SA_SIZE   space_time[3] array_part[16,16,16] latency[8,8] simd[2]
    CONFIG    ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
)

add_dependencies(mm-int8-tapa heterosa_cc)
get_target_property(TAPA_PATH mm-int8-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL mm-int8-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(mm-int8)
    add_dependencies(mm-int8 mm-int8-tapa)
    target_sources(mm-int8 PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(mm-int8 PUBLIC tapa::tapa frt::frt)
    add_test(NAME mm-int8-test COMMAND mm-int8)
endif()