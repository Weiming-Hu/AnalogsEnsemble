cmake_minimum_required(VERSION 2.8)
project(TestStations)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testStations "testStations.cpp")
add_executable(runStations "runnerStations.cpp")

add_dependencies(testStations AnEnIO AnEn)
add_dependencies(runStations AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testStations ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runStations ${UnitTestLibs} testStations AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testStations cppunit)
add_dependencies(runStations cppunit)

target_link_libraries (testStations cppunit AnEnIO AnEn)
target_link_libraries (runStations cppunit testStations AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runStations PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runStations)
