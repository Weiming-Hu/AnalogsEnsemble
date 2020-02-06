cmake_minimum_required(VERSION 2.8)
project(TestFunctions)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testFunctions "testFunctions.cpp")
add_executable(runFunctions "runnerFunctions.cpp")

add_dependencies(testFunctions AnEnIO AnEn)
add_dependencies(runFunctions AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testFunctions ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runFunctions ${UnitTestLibs} testFunctions AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testFunctions cppunit)
add_dependencies(runFunctions cppunit)

target_link_libraries (testFunctions cppunit AnEnIO AnEn)
target_link_libraries (runFunctions cppunit testFunctions AnEnIO AnEn)
endif(CPPUNIT_FOUND)

add_definitions(-D_PATH_OBSERVATIONS="${CMAKE_SOURCE_DIR}/tests/Data/test_observations.nc"
    -D_PATH_FORECASTS="${CMAKE_SOURCE_DIR}/tests/Data/test_forecasts.nc")

set_target_properties(runFunctions PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runFunctions)
install(TARGETS runFunctions RUNTIME DESTINATION bin)
