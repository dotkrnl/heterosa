include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    mm-hbm-tapa
    INPUT     kernel.c
    SIMD_INFO simd_info.json
    SA_SIZE   space_time[3] array_part[16,16,16] latency[8,8] simd[2] hbm_A[2] hbm_B[2] hbm_C_drain[2]
    CONFIG    ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
    --hbm --no-host-serialize
)

add_dependencies(mm-hbm-tapa heterosa_cc)
get_target_property(TAPA_PATH mm-hbm-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL mm-hbm-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(mm-hbm)
    add_dependencies(mm-hbm mm-hbm-tapa)
    target_sources(mm-hbm PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(mm-hbm PUBLIC tapa::tapa frt::frt)
    add_test(NAME mm-hbm-test COMMAND mm-hbm)
endif()