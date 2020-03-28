###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################

# This file builds the AnEnMPI

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(AnEnMPI VERSION ${GRAND_VERSION} LANGUAGES CXX)
message(STATUS "Configuring the class ${PROJECT_NAME}")

# Find the dependent libraries
find_package(MPI REQUIRED)
find_package(AnEn REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS program_options)

add_library(AnEnMPI ${CMAKE_CURRENT_SOURCE_DIR}/AnEnISMPI.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FunctionsMPI.cpp)

add_library(AnEnMPI::AnEnMPI ALIAS AnEnMPI)

target_include_directories(AnEnMPI SYSTEM PUBLIC ${MPI_INCLUDE_PATH})
target_include_directories(AnEnMPI BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(AnEnMPI PUBLIC ${MPI_CXX_LIBRARIES} AnEn::AnEn)

install(TARGETS AnEnMPI 
    EXPORT AnEnMPIConfig
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

install(EXPORT AnEnMPIConfig
    NAMESPACE AnEnMPI::
    DESTINATION lib/cmake)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/AnEnISMPI.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/FunctionsMPI.h"
    DESTINATION include)


