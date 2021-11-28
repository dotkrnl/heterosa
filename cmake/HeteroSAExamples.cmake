set(HETEROSA_BIN PYTHONPATH=${CMAKE_SOURCE_DIR}/frontend python3 -m heterosa.cli)
set(HETEROSA_CC_PATH ${CMAKE_BINARY_DIR}/backend)
find_package(Boost 1.59 COMPONENTS REQUIRED coroutine stacktrace_basic)
include("${CMAKE_CURRENT_LIST_DIR}/HeteroSAConfig.cmake")