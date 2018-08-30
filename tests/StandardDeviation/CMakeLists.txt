cmake_minimum_required(VERSION 2.8)
project(TestStandardDeviation)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testStandardDeviation "testStandardDeviation.cpp")
add_executable(runStandardDeviation "runnerStandardDeviation.cpp")

add_dependencies(testStandardDeviation AnEnIO AnEn)
add_dependencies(runStandardDeviation AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testStandardDeviation ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runStandardDeviation ${UnitTestLibs} testStandardDeviation AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testStandardDeviation cppunit)
add_dependencies(runStandardDeviation cppunit)

target_link_libraries (testStandardDeviation cppunit AnEnIO AnEn)
target_link_libraries (runStandardDeviation cppunit testStandardDeviation AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runStandardDeviation PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runStandardDeviation)
install(TARGETS runStandardDeviation RUNTIME DESTINATION bin)
