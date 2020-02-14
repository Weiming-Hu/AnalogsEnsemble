cmake_minimum_required(VERSION 3.12)
project(testPAnEn VERSION ${GRAND_VERSION} LANGUAGES C CXX)
enable_testing()

# Find the dependency package
find_package(CppUnit)

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

