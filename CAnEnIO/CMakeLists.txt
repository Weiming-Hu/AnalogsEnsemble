###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Institute for Computational and Data Science                            #
#         Department of Geography                                                 #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This file adds all source files and export the AnEnIO library
# 
# AnEnIO library depends on AnEn, so this project should within the same project
# as the AnEn library.
#

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project (AnEnIO VERSION ${GRAND_VERSION} LANGUAGES C CXX)
message(STATUS "Configuring the library AnEnIO")

# Define AnEnIO source files
set(AnEnIO_source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnEnReadGrib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnEnReadNcdf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnEnWriteNcdf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FunctionsIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Ncdf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ParameterGrib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Txt.cpp)

# Define AnEnIO headers
set(AnEnIO_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEnReadGrib.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEnReadNcdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEnReadNcdf.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEnWriteNcdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/FunctionsIO.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Ncdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Ncdf.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ParameterGrib.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Txt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Txt.tpp)

# We have provided some cmake modules to find libraries
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find the dependent libraries and components
set(NETCDF_CXX "YES")
find_package(AnEn REQUIRED)
find_package(eccodes REQUIRED)
find_package(NetCDF REQUIRED)
find_package(Boost 1.58.0 REQUIRED COMPONENTS date_time filesystem)

# Create target by explicitly listing all source files
add_library(AnEnIO ${AnEnIO_source_files})

# Create an alias target so that this works in sub directories
add_library(AnEnIO::AnEnIO ALIAS AnEnIO)

# Configure the properties of this target
target_link_libraries(AnEnIO
    PUBLIC AnEn::AnEn Boost::date_time ${NETCDF_LIBRARIES}
    PRIVATE Boost::filesystem ${ECCODES_LIBRARIES})

target_include_directories(AnEnIO SYSTEM PUBLIC 
    ${ECCODES_INCLUDES} ${NETCDF_INCLUDES})

target_include_directories(AnEnIO BEFORE PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../CAnEn/include>
    $<INSTALL_INTERFACE:include>)

install(TARGETS AnEnIO 
    EXPORT AnEnIOConfig
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

install(EXPORT AnEnIOConfig
    NAMESPACE AnEnIO::
    DESTINATION lib/cmake)

install(FILES ${AnEnIO_headers} DESTINATION include)

