###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################

# This file builds the utility anen_netcdf. This target depends on targets AnEn and AnEnIO.

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(anen_netcdf VERSION ${GRAND_VERSION} LANGUAGES CXX)
message(STATUS "Configuring the executable ${PROJECT_NAME}")

# Find the dependent libraries
find_package(AnEnIO)
find_package(Boost 1.58.0 REQUIRED COMPONENTS program_options)

# Create target
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/anen_netcdf.cpp)

# Configure the properties of this target
target_link_libraries(${PROJECT_NAME} PRIVATE AnEnIO::AnEnIO Boost::program_options)

# Export the executable
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets RUNTIME DESTINATION bin)

