###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################

# This file builds the utility anen. This target depends on targets AnEn and AnEnIO.

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(anen VERSION ${GRAND_VERSION} LANGUAGES CXX)
message(STATUS "Configuring the executable anen")

# Find the dependent libraries
find_package(AnEn)
find_package(AnEnIO)
find_package(Boost 1.65.0 REQUIRED COMPONENTS program_options)

# Create target
add_executable(anen ${CMAKE_CURRENT_SOURCE_DIR}/anen.cpp)

# Configure the properties of this target
target_link_libraries(anen
    PUBLIC
    AnEnIO::AnEnIO Boost::program_options)

# Export the executable
install(TARGETS anen
    EXPORT anenTargets
    RUNTIME DESTINATION bin)

#set(SOURCE_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/anen.cpp")
#add_executable(${PROJECT_NAME} ${SOURCE_MAIN})
#
#include_directories(SYSTEM ${NETCDF_INCLUDES})
#include_directories(SYSTEM ${NETCDF_CXX4_INCLUDE_DIR})
#include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
#add_dependencies(${PROJECT_NAME} AnEnIO)
#target_link_libraries(${PROJECT_NAME} AnEnIO)
#
#include_directories("${CMAKE_SOURCE_DIR}/CAnEn/include")
#add_dependencies(${PROJECT_NAME} AnEn)
#target_link_libraries(${PROJECT_NAME} AnEn)
#
#set_target_properties(anen
#    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")
#install(TARGETS anen RUNTIME DESTINATION bin)
