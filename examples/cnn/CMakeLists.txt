include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/HeteroSAExamples.cmake)

add_heterosa_target(
    cnn-tapa
    INPUT     kernel.c
    SIMD_INFO simd_info.json
    SA_SIZE   space_time[4] array_part[8,8,4,8] latency[4,2,4] simd[1,1,1,2]
    CONFIG    ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/heterosa_config.json
)

add_dependencies(cnn-tapa heterosa_cc)
get_target_property(TAPA_PATH cnn-tapa OUTPUT_PATH)
get_target_property(TAPA_KERNEL cnn-tapa KERNEL)

find_package(TAPA)
find_package(FRT)
if(TAPA_FOUND AND FRT_FOUND)
    add_executable(cnn)
    add_dependencies(cnn cnn-tapa)
    target_sources(cnn PRIVATE ${TAPA_PATH}/src/kernel_host.cpp ${TAPA_PATH}/src/kernel_kernel.cpp)
    set_source_files_properties(${TAPA_PATH}/src/kernel_host.cpp PROPERTIES GENERATED 1)
    set_source_files_properties(${TAPA_PATH}/src/kernel_kernel.cpp PROPERTIES GENERATED 1)
    target_link_libraries(cnn PUBLIC tapa::tapa frt::frt)
    add_test(NAME cnn-test COMMAND cnn)
endif()