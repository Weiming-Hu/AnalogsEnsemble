###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This file adds all source files and export the AnEnIO library
# 
# AnEnIO library depends on AnEn, so this project should within the same project
# as the AnEn library.
#

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
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
find_package(Boost 1.65.0 REQUIRED COMPONENTS filesystem program_options)

# Create target by explicitly listing all source files
add_library(AnEnIO ${AnEnIO_source_files})

# Create an alias target so that this works in sub directories
add_library(AnEnIO::AnEnIO ALIAS AnEnIO)

# Configure the properties of this target
target_link_libraries(AnEnIO
    PUBLIC
    Boost::program_options Boost::filesystem AnEn::AnEn
    ${ECCODES_LIBRARIES} ${NETCDF_LIBRARIES_CXX})

target_include_directories(AnEnIO
    PUBLIC 
    ${ECCODES_INCLUDES} ${NETCDF_INCLUDES}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}../AnEn/include>
    $<INSTALL_INTERFACE:include>)

if(NOT BUILD_BOOST)

    # Since this project dependends on boost-cmake which does not support installation
    # yet, if boost was built, the libraries that depend on the built boost are not
    # installed.
    # 
    install(TARGETS AnEnIO EXPORT AnEnIOTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

    install(FILES ${AnEnIO_headers} DESTINATION include)

    install(EXPORT AnEnIOTargets
        FILE AnEnIOTargets.cmake
        NAMESPACE AnEnIO::
        DESTINATION lib/cmake)

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)

endif(NOT BUILD_BOOST)

