cmake_minimum_required(VERSION 2.8)
project(ParallelEnsembleForecastsTests)
enable_language(CXX C)

# Include libraries
include_directories(${NETCDF_INCLUDES})
include_directories(${NETCDF_CXX4_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
include_directories("${CMAKE_SOURCE_DIR}/CAnEn/include")

# Add all the tests
add_subdirectory("ObservationsArray")
add_subdirectory("ForecastsArray")
#add_subdirectory("Functions")
add_subdirectory("Parameters")
add_subdirectory("Stations")
add_subdirectory("Station")
#add_subdirectory("AnEnIO")
add_subdirectory("Times")
#add_subdirectory("AnEn")

if (BUILD_GRIBCONVERTER)
    message(STATUS "Build test for GRIB converter.")
    add_subdirectory("gribConverter")
endif (BUILD_GRIBCONVERTER)

