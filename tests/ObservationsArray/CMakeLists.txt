cmake_minimum_required(VERSION 2.8)
project(TestObservationsArray)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testObservationsArray "testObservationsArray.cpp")
add_executable(runObservationsArray "runnerObservationsArray.cpp")

add_dependencies(testObservationsArray AnEnIO AnEn)
add_dependencies(runObservationsArray AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testObservationsArray ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runObservationsArray ${UnitTestLibs} testObservationsArray AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testObservationsArray cppunit)
add_dependencies(runObservationsArray cppunit)

target_link_libraries (testObservationsArray cppunit AnEnIO AnEn)
target_link_libraries (runObservationsArray cppunit testObservationsArray AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runObservationsArray PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runObservationsArray)
install(TARGETS runObservationsArray RUNTIME DESTINATION bin)
