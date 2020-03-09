###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################

# This file builds the AnEnIOMPI

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(AnEnIOMPI VERSION ${GRAND_VERSION} LANGUAGES CXX)
message(STATUS "Configuring the class ${PROJECT_NAME}")

# Find the dependent libraries
find_package(MPI REQUIRED)
find_package(AnEnIO REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS program_options)

add_library(AnEnIOMPI ${CMAKE_CURRENT_SOURCE_DIR}/AnEnReadGribMPI.cpp)

add_library(AnEnIOMPI::AnEnIOMPI ALIAS AnEnIOMPI)

target_include_directories(AnEnIO SYSTEM PUBLIC ${MPI_INCLUDE_PATH})

target_link_libraries(AnEnIOMPI PUBLIC AnEnIO::AnEnIO ${MPI_CXX_LIBRARIES})

target_include_directories(AnEnIO BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(TARGETS AnEnIOMPI 
    EXPORT AnEnIOMPIConfig
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

install(EXPORT AnEnIOMPIConfig
    NAMESPACE AnEnIOMPI::
    DESTINATION lib/cmake)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/AnEnIOMPI.h" DESTINATION include)

