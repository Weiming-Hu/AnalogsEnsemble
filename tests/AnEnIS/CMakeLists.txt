cmake_minimum_required(VERSION 2.8)
project(TestAnEnIS)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testAnEnIS "testAnEnIS.cpp")
add_executable(runAnEnIS "runnerAnEnIS.cpp")

add_dependencies(testAnEnIS AnEnIO AnEn)
add_dependencies(runAnEnIS AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testAnEnIS ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runAnEnIS ${UnitTestLibs} testAnEnIS AnEnIO AnEn)
else(CPPUNIT_FOUND)
    add_dependencies(testAnEnIS cppunit)
    add_dependencies(runAnEnIS cppunit)

    target_link_libraries (testAnEnIS cppunit AnEnIO AnEn)
    target_link_libraries (runAnEnIS cppunit testAnEnIS AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runAnEnIS PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runAnEnIS)
install(TARGETS runAnEnIS RUNTIME DESTINATION bin)
