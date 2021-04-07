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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GridQuery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Grid.cpp)

# Define header files
set(Grid_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GridQuery.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Grid.h)

# Create target by explicitly listing all source files
add_library(Grid ${Grid_source_files})

# Create an alias target so that this works in sub directories
add_library(Grid::Grid ALIAS Grid)

# Configure the properties of this target
target_include_directories(Grid PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(Grid PUBLIC AnEnIO::AnEnIO)

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

###################################
# Build Python API for Grid class #
###################################

# To build the API for the Grid class, I did not directly use the 
# target Grid. Instead, I will create a second target, called GridOnly.
#
# The reason is to avoid shared library conflict. Grid has several
# components, of which one (GridQuery) depends on AnEn. AnEn depends
# on Torch. However, this Torch will conflict with the PyTorch built
# by pip. There, in python, `import torch` will conflict with
# `import AnEnGrid` because they each depend on a different libTorch
# library.
#
# Therefore, since GridQuery is not needed in the API. Luckily, I 
# can take that off the API and build GridOnly that does not depend
# on AnEn. If you check the linked libraries to GridOnly, there is
# nothing related to either AnEn or libTorch. As a result, importing
# both torch and AnEnGrid in python won't conflict.

message(STATUS "Configuring the library AnEnGrid")
message(STATUS "Python exec: ${PYTHON_EXECUTABLE}")

# Define the source files
set(GridOnly_source_files 
    ${CMAKE_CURRENT_SOURCE_DIR}/../CAnEnIO/src/Txt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Grid.cpp)

# Create target by explicitly listing all source files
add_library(GridOnly ${GridOnly_source_files})

# Create an alias target so that this works in sub directories
add_library(GridOnly::GridOnly ALIAS GridOnly)

target_include_directories(GridOnly PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../CAnEnIO/include)

# Configure the properties of this target
target_include_directories(GridOnly PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

find_package(Boost 1.58.0 REQUIRED COMPONENTS filesystem)
target_link_libraries(GridOnly PUBLIC Boost::boost Boost::filesystem)

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
target_link_libraries(AnEnGrid PUBLIC GridOnly)

