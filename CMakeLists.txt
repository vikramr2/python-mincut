cmake_minimum_required(VERSION 3.2)

find_program(GCC NAMES gcc)
find_program(GPP NAMES g++)


set(CMAKE_CXX_COMPILER "${GPP}")
set(CMAKE_C_COMPILER "${GCC}")
project(python_mincut)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# find_package(OpenMP REQUIRED)
# find_package(MPI REQUIRED)
# include_directories(SYSTEM ${MPI_INCLUDE_PATH})

OPTION(RUN_TESTS "Compile and run tests" ON)

if(NOT CMAKE_BUILD_TYPE)
  message("Defaulting CMAKE_BUILD_TYPE to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# prohibit in-source builds
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(SEND_ERROR "In-source builds are not allowed.")
endif()

MESSAGE(STATUS "Option: RUN_TESTS " ${RUN_TESTS})
MESSAGE(STATUS "Option: USE_TCMALLOC " ${USE_TCMALLOC})
MESSAGE(STATUS "Option: USE_PROFILER " ${USE_PROFILER})
MESSAGE(STATUS "Option: USE_GUROBI " ${USE_GUROBI})

MESSAGE(STATUS "GUROBI INCLUDE ${GUROBI_INCLUDE_DIR}")
MESSAGE(STATUS "TCMALLOC INCLUDE ${Tcmalloc_INCLUDE_DIR}")
add_subdirectory(extern/VieCut/extlib/tlx)

include_directories(
    extern/VieCut/app 
    extern/VieCut/lib             
    # /usr/include/mpi
    extern/VieCut/extlib/growt
    extern/VieCut/extlib/tlx
    ${GUROBI_INCLUDE_DIR}
    ${Tcmalloc_INCLUDE_DIR}
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set (CMAKE_CXX_FLAGS_DEBUG "-g3 -march=native")

set(EXTLIBS tlx)

if(USE_PROFILER)
set(LIBS ${EXTLIBS} ${Profiler_LIBRARIES} ${GUROBI_LIBRARIES})
else()
set(LIBS ${EXTLIBS} ${Tcmalloc_LIBRARIES} ${GUROBI_LIBRARIES})
endif()

add_subdirectory(extern/pybind11)

pybind11_add_module(mincut_wrapper src/mincut_wrapper.cpp)
target_link_libraries(mincut_wrapper PUBLIC ${LIBS})