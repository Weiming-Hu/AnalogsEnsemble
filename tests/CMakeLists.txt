cmake_minimum_required(VERSION 2.8)
project(testPAnEn)
enable_language(CXX C)
message(STATUS "Building tests")

# Include libraries
include_directories(${NETCDF_INCLUDES})
include_directories(${NETCDF_CXX4_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
include_directories("${CMAKE_SOURCE_DIR}/CAnEn/include")

# Add all the tests
add_subdirectory("ObservationsPointer")
add_subdirectory("ForecastsPointer")
add_subdirectory("Calculator")
add_subdirectory("Parameters")
add_subdirectory("Functions")
add_subdirectory("FunctionsIO")
add_subdirectory("Stations")
add_subdirectory("Station")
add_subdirectory("Times")
add_subdirectory("AnEnIS")
add_subdirectory("AnEnSSE")

