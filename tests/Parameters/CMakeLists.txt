cmake_minimum_required(VERSION 2.8)
project(TestParameters)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(TestParameters "testParameters.cpp")
add_executable(runParameters "runnerParameters.cpp")

add_dependencies(TestParameters AnEnIO AnEn)
add_dependencies(runParameters AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (TestParameters ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runParameters ${UnitTestLibs} TestParameters AnEnIO AnEn)
else(CPPUNIT_FOUND)
    add_dependencies(TestParameters cppunit)
    add_dependencies(runParameters cppunit)

    target_link_libraries (TestParameters cppunit AnEnIO AnEn)
    target_link_libraries (runParameters cppunit TestParameters AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runParameters PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runParameters)
install(TARGETS runParameters RUNTIME DESTINATION bin)
