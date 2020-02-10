cmake_minimum_required(VERSION 2.8)
project(TestFunctionsIO)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testFunctionsIO "testFunctionsIO.cpp")
add_executable(runFunctionsIO "runnerFunctionsIO.cpp")

add_dependencies(testFunctionsIO AnEnIO AnEn)
add_dependencies(runFunctionsIO AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testFunctionsIO ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runFunctionsIO ${UnitTestLibs} testFunctionsIO AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testFunctionsIO cppunit)
add_dependencies(runFunctionsIO cppunit)

target_link_libraries (testFunctionsIO cppunit AnEnIO AnEn)
target_link_libraries (runFunctionsIO cppunit testFunctionsIO AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runFunctionsIO PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runFunctionsIO)
install(TARGETS runFunctionsIO RUNTIME DESTINATION bin)
