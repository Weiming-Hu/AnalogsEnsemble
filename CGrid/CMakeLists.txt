###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This file adds all source files and export the Grid library

project(Grid VERSION ${GRAND_VERSION} LANGUAGES CXX)
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
message(STATUS "Configuring the library Grid")

# Define the source files
set(Grid_source_files 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Grid.cpp)

# Define header files
set(Grid_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Grid.h)

# Find the dependent library and components
#find_package(Boost 1.58.0 REQUIRED COMPONENTS date_time serialization)

# Create target by explicitly listing all source files
add_library(Grid ${Grid_source_files})

# Create an alias target so that this works in sub directories
add_library(Grid::Grid ALIAS Grid)

# Configure the properties of this target
target_include_directories(Grid PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(Grid 
    PUBLIC AnEnIO::AnEnIO)

#if(ENABLE_AI)
#    target_compile_features(AnEn PUBLIC cxx_std_14)
#    target_link_libraries(AnEn PUBLIC torch)
#else(ENABLE_AI)
#    target_compile_features(AnEn PUBLIC cxx_std_11)
#endif(ENABLE_AI)

#if(ENABLE_OPENMP)
#    find_package(OpenMP)
#
#    if(OpenMP_CXX_FOUND)
#        target_link_libraries(AnEn PUBLIC OpenMP::OpenMP_CXX)
#    else(OpenMP_CXX_FOUND)
#        message(WARNING "OpenMP (multi-threading) is not found")
#    endif(OpenMP_CXX_FOUND)
#
#endif(ENABLE_OPENMP)

install(TARGETS Grid
    EXPORT GridConfig
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

install(EXPORT GridConfig
    NAMESPACE Grid::
    DESTINATION lib/cmake)

install(FILES ${Grid_headers} DESTINATION include)

# Build Python API
message(STATUS "Configuring the library AnEnGrid")
message(STATUS "Python exec: ${PYTHON_EXECUTABLE}")

if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pybind11")
    message(STATUS "Downloading pybind11 source with git ...")
    find_package(Git REQUIRED QUIET)
    execute_process(COMMAND ${GIT_EXECUTABLE} "clone"
        "https://github.com/pybind/pybind11.git"
        "${CMAKE_CURRENT_SOURCE_DIR}/pybind11")
else ()
    message(STATUS "pybind11 already exists at ${CMAKE_CURRENT_SOURCE_DIR}/pybind11")
endif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pybind11")

add_subdirectory(pybind11)
pybind11_add_module(AnEnGrid ${CMAKE_CURRENT_SOURCE_DIR}/src/GridPy.cpp)
target_link_libraries(AnEnGrid PUBLIC Grid)

