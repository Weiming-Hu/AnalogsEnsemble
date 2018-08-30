cmake_minimum_required(VERSION 2.8)
project(TestAnEn)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testAnEn "testAnEn.cpp")
add_executable(runAnEn "runnerAnEn.cpp")

add_dependencies(testAnEn AnEnIO AnEn)
add_dependencies(runAnEn AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testAnEn ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runAnEn ${UnitTestLibs} testAnEn AnEnIO AnEn)
else(CPPUNIT_FOUND)
    add_dependencies(testAnEn cppunit)
    add_dependencies(runAnEn cppunit)

    target_link_libraries (testAnEn cppunit AnEnIO AnEn)
    target_link_libraries (runAnEn cppunit testAnEn AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runAnEn PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runAnEn)
install(TARGETS runAnEn RUNTIME DESTINATION bin)
