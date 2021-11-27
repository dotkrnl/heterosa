if(PROJECT_NAME STREQUAL "heterosa")
  set(TAPAC PYTHONPATH=${CMAKE_SOURCE_DIR}/frontend python3 -m heterosa.cli)
  set(HETEROSA_CC_PATH ${CMAKE_BINARY_DIR}/backend)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/HeteroSAConfig.cmake")