###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This file adds all source files and export the AnEn library

project(AnEn VERSION ${GRAND_VERSION} LANGUAGES CXX)
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
message(STATUS "Configuring the library AnEn")

# Define the source files
set(AnEn_source_files 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnEn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnEnIS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnEnSSE.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AnEnSSEMS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Array4DPointer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/BasicData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Calculator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Forecasts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ForecastsPointer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Functions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Observations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ObservationsPointer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Parameters.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Profiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Stations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Times.cpp)

# Define header files
set(AnEn_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEn.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEnIS.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEnIS.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEnSSE.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEnSSE.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/AnEnSSEMS.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Array4D.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Array4DPointer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/BasicData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/BmDim.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Calculator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Forecasts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ForecastsPointer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Functions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Functions.tpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Observations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ObservationsPointer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Parameters.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Profiler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Stations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Times.h)

# Find the dependent library and components
find_package(Boost 1.58.0 REQUIRED COMPONENTS date_time serialization)

# Create target by explicitly listing all source files
add_library(AnEn ${AnEn_source_files})

# Create an alias target so that this works in sub directories
add_library(AnEn::AnEn ALIAS AnEn)

# Configure the properties of this target
target_include_directories(AnEn PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(AnEn PUBLIC Grid::Grid Boost::boost Boost::serialization Boost::date_time)

if(ENABLE_AI)
    target_compile_features(AnEn PUBLIC cxx_std_14)
    target_link_libraries(AnEn PUBLIC torch)
else(ENABLE_AI)
    target_compile_features(AnEn PUBLIC cxx_std_11)
endif(ENABLE_AI)

if(ENABLE_OPENMP)
    find_package(OpenMP)

    if(OpenMP_CXX_FOUND)
        target_link_libraries(AnEn PUBLIC OpenMP::OpenMP_CXX)
    else(OpenMP_CXX_FOUND)
        message(WARNING "OpenMP (multi-threading) is not found")
    endif(OpenMP_CXX_FOUND)

endif(ENABLE_OPENMP)

install(TARGETS AnEn
    EXPORT AnEnConfig
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

install(EXPORT AnEnConfig
    NAMESPACE AnEn::
    DESTINATION lib/cmake)

install(FILES ${AnEn_headers} DESTINATION include)
