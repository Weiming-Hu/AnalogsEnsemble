cmake_minimum_required(VERSION 2.8)
project(TestForecastsArray)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testForecastsArray "testForecastsArray.cpp")
add_executable(runForecastsArray "runnerForecastsArray.cpp")

add_dependencies(testForecastsArray AnEnIO AnEn)
add_dependencies(runForecastsArray AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testForecastsArray ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runForecastsArray ${UnitTestLibs} testForecastsArray AnEnIO AnEn)
else(CPPUNIT_FOUND)
    add_dependencies(testForecastsArray cppunit)
    add_dependencies(runForecastsArray cppunit)

    target_link_libraries (testForecastsArray cppunit AnEnIO AnEn)
    target_link_libraries (runForecastsArray cppunit testForecastsArray AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runForecastsArray PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runForecastsArray)
install(TARGETS runForecastsArray RUNTIME DESTINATION bin)
