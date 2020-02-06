cmake_minimum_required(VERSION 2.8)
project(TestObservationsPointer)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testObservationsPointer "testObservationsPointer.cpp")
add_executable(runObservationsPointer "runnerObservationsPointer.cpp")

add_dependencies(testObservationsPointer AnEnIO AnEn)
add_dependencies(runObservationsPointer AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testObservationsPointer ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runObservationsPointer ${UnitTestLibs} testObservationsPointer AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testObservationsPointer cppunit)
add_dependencies(runObservationsPointer cppunit)

target_link_libraries (testObservationsPointer cppunit AnEnIO AnEn)
target_link_libraries (runObservationsPointer cppunit testObservationsPointer AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runObservationsPointer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runObservationsPointer)
install(TARGETS runObservationsPointer RUNTIME DESTINATION bin)
