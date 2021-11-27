include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    mm-tapa
    INPUT     kernel.c
    SIMD_INFO simd_info.json
    SA_SIZE   space_time[3] array_part[16,16,16] latency[8,8] simd[2]
    CONFIG    ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
    --host-serialize
)

add_dependencies(mm-tapa heterosa_cc)
get_target_property(TAPA_PATH mm-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL mm-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(mm)
    add_dependencies(mm mm-tapa)
    target_sources(mm PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(mm PUBLIC tapa::tapa frt::frt)
    add_test(NAME mm-test COMMAND mm)
endif()