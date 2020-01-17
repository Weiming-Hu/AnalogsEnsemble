cmake_minimum_required(VERSION 2.8)
project(TestCalculator)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testCalculator "testCalculator.cpp")
add_executable(runCalculator "runnerCalculator.cpp")

add_dependencies(testCalculator AnEnIO AnEn)
add_dependencies(runCalculator AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testCalculator ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runCalculator ${UnitTestLibs} testCalculator AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testCalculator cppunit)
add_dependencies(runCalculator cppunit)

target_link_libraries (testCalculator cppunit AnEnIO AnEn)
target_link_libraries (runCalculator cppunit testCalculator AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runCalculator PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runCalculator)
install(TARGETS runCalculator RUNTIME DESTINATION bin)
