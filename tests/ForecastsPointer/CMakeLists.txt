cmake_minimum_required(VERSION 2.8)
project(TestForecastsPointer)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testForecastsPointer "testForecastsPointer.cpp")
add_executable(runForecastsPointer "runnerForecastsPointer.cpp")

add_dependencies(testForecastsPointer AnEnIO AnEn)
add_dependencies(runForecastsPointer AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testForecastsPointer ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runForecastsPointer ${UnitTestLibs} testForecastsPointer AnEnIO AnEn)
else(CPPUNIT_FOUND)
    add_dependencies(testForecastsPointer cppunit)
    add_dependencies(runForecastsPointer cppunit)

    target_link_libraries (testForecastsPointer cppunit AnEnIO AnEn)
    target_link_libraries (runForecastsPointer cppunit testForecastsPointer AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runForecastsPointer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runForecastsPointer)
install(TARGETS runForecastsPointer RUNTIME DESTINATION bin)
