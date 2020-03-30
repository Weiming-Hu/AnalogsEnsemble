cmake_minimum_required(VERSION 3.10)
project(testPAnEn VERSION ${GRAND_VERSION} LANGUAGES C CXX)

add_custom_target(test_all ${CMAKE_CTEST_COMMAND} -V)

# Find the dependency package
find_package(AnEnIO REQUIRED)

# Some tests need data files which are defined using definitions
add_definitions(-D_PATH_OBSERVATIONS="${CMAKE_CURRENT_SOURCE_DIR}/Data/test_observations.nc")
add_definitions(-D_PATH_FORECASTS="${CMAKE_CURRENT_SOURCE_DIR}/Data/test_forecasts.nc")
add_definitions(-D_TEST_DATA_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/Data/")

# Add tests
PAnEn_test_this("ObservationsPointer")
PAnEn_test_this("ForecastsPointer")
PAnEn_test_this("Calculator")
PAnEn_test_this("Parameters")
PAnEn_test_this("Functions")
PAnEn_test_this("FunctionsIO")
PAnEn_test_this("Stations")
PAnEn_test_this("Station")
PAnEn_test_this("Times")
PAnEn_test_this("AnEnIS")
PAnEn_test_this("AnEnSSE")

if(ENABLE_MPI)
    find_package(AnEnIOMPI)
    find_package(AnEnMPI)

    PAnEn_test_this("AnEnIOMPI")
    PAnEn_test_this("AnEnMPI")

    # Link to extra libraries
    target_link_libraries(testAnEnIOMPI PUBLIC AnEnIOMPI::AnEnIOMPI)
    target_link_libraries(testAnEnMPI PUBLIC AnEnMPI::AnEnMPI)
endif(ENABLE_MPI)

