function(add_heterosa_target target_name)
    # Generate TAPA project from HeteroSA C.
    #
    # The added target will have the following properties:
    #
    # * OUTPUT_PATH
    # * KERNEL
    #
    # Positional Arguments:
    #
    # * target_name: Name of the added cmake target.
    #
    # Required Named Arguments:
    #
    # * INPUT: Input filename.
    #
    # Optional Named Arguments:
    #
    # * OUTPUT: Optional, output filename, default to ${target_name}.prj
    # * SIMD_INFO: Optional, path to the SIMD information file of the input kernel.
    # * CONFIG: Optional, path to the HeteroSA configuration file.
    # * SA_SIZE: Optional, configurations for the systolic array.
    # * DATAPACK_SIZE: Optional, configurations for the data packing.
    # * HETEROSA: Optional, command to run the `heterosa` executable.
    # * HETEROSA_CC_PATH: Optional, directory of the `heterosa_cc` executable.
    #
    # Other Arguments:
    #
    # * Optional, will be passed to `heterosa` directly.

    cmake_parse_arguments(
        HETEROSA
        ""
        "INPUT;OUTPUT;SIMD_INFO;CONFIG;HETEROSA;HETEROSA_CC_PATH"
        "SA_SIZE;DATAPACK_SIZE"
        ${ARGN})

    if(NOT HETEROSA_INPUT)
        message(FATAL_ERROR "INPUT not specified")
    endif()
    if(NOT HETEROSA_OUTPUT)
        set(HETEROSA_OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/${target_name}.prj)
    endif()
    get_filename_component(HETEROSA_INPUT ${HETEROSA_INPUT} ABSOLUTE)
    get_filename_component(HETEROSA_OUTPUT ${HETEROSA_OUTPUT} ABSOLUTE)

    if(HETEROSA_HETEROSA_CC_PATH)
        set(HETEROSA_CC_PATH ${HETEROSA_HETEROSA_CC_PATH})
        find_program(HETEROSA_CC heterosa_cc PATHS ${HETEROSA_CC_PATH} NO_DEFAULT_PATH)
    else()
        find_program(HETEROSA_CC heterosa_cc)
    endif()
    if(NOT HETEROSA_CC)
        message(FATAL_ERROR "cannot find heterosa_cc")
    endif()

    if(HETEROSA_HETEROSA)
        set(HETEROSA_BIN ${HETEROSA_HETEROSA})
    else()
        find_program(HETEROSA_BIN heterosa)
    endif()
    if(NOT HETEROSA_BIN)
        message(FATAL_ERROR "cannot find heterosa")
    endif()

    set(heterosa_cmd ${HETEROSA_BIN} ${HETEROSA_INPUT})
    if(HETEROSA_CC_PATH)
        list(PREPEND heterosa_cmd PATH=${HETEROSA_CC_PATH}:/usr/bin/:/bin/)
    endif()
    if(HETEROSA_OUTPUT)
        get_filename_component(HETEROSA_OUTPUT ${HETEROSA_OUTPUT} ABSOLUTE)
        list(APPEND heterosa_cmd --output-dir ${HETEROSA_OUTPUT})
    endif()
    if(HETEROSA_SIMD_INFO)
        get_filename_component(HETEROSA_SIMD_INFO ${HETEROSA_SIMD_INFO} ABSOLUTE)
        list(APPEND heterosa_cmd --simd-info ${HETEROSA_SIMD_INFO})
    endif()
    if(HETEROSA_CONFIG)
        get_filename_component(HETEROSA_CONFIG ${HETEROSA_CONFIG} ABSOLUTE)
        list(APPEND heterosa_cmd --config ${HETEROSA_CONFIG})
    endif()
    if(HETEROSA_SA_SIZE)
        list(APPEND heterosa_cmd --sa-sizes)
        set(SA_SIZE "{")
        foreach(SA IN LISTS HETEROSA_SA_SIZE)
            set(SA_SIZE "${SA_SIZE}kernel[]->${SA}\\;")
        endforeach()
        string(REGEX REPLACE "\\\\;$" "" SA_SIZE "${SA_SIZE}")
        set(SA_SIZE "${SA_SIZE}}")
        list(APPEND heterosa_cmd "${SA_SIZE}")
    endif()
    if(HETEROSA_DATAPACK_SIZE)
        list(APPEND heterosa_cmd --datapack-sizes)
        set(DATAPACK_SIZE "{")
        foreach(DATAPACK IN LISTS HETEROSA_DATAPACK_SIZE)
            set(DATAPACK_SIZE "${DATAPACK_SIZE}kernel[]->${DATAPACK}\\;")
        endforeach()
        string(REGEX REPLACE "\\\\;$" "" DATAPACK_SIZE "${DATAPACK_SIZE}")
        set(DATAPACK_SIZE "${DATAPACK_SIZE}}")
        list(APPEND heterosa_cmd "${DATAPACK_SIZE}")
    endif()
    list(APPEND heterosa_cmd ${HETEROSA_UNPARSED_ARGUMENTS})
    #list(APPEND heterosa_cmd --quiet)

    add_custom_command(
        OUTPUT  ${HETEROSA_OUTPUT}
        COMMAND ${heterosa_cmd}
        DEPENDS ${HETEROSA_INPUT}
        COMMENT "Generating TAPA project ${HETEROSA_OUTPUT} from ${HETEROSA_INPUT}"
        VERBATIM
    )
    add_custom_target(${target_name} DEPENDS ${HETEROSA_OUTPUT})

    set_target_properties(
        ${target_name}
        PROPERTIES
        OUTPUT_PATH ${HETEROSA_OUTPUT}
        KERNEL "kernel0"
    )
endfunction()
