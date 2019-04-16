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

if(${ENABLE_MPI})
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
    target_link_libraries(testStandardDeviation ${MPI_CXX_LIBRARIES})
    if(MPI_CXX_COMPILE_FLAGS)
        set_target_properties(testStandardDeviation PROPERTIES
            COMPILE_FLAGS ${MPI_CXX_COMPILE_FLAGS})
    endif(MPI_CXX_COMPILE_FLAGS)
    if(MPI_CXX_LINK_FLAGS)
        set_target_properties(testStandardDeviation PROPERTIES
            LINK_FLAGS ${MPI_CXX_LINK_FLAGS})
    endif(MPI_CXX_LINK_FLAGS)
endif(${ENABLE_MPI}) 

set_target_properties(runStandardDeviation PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runStandardDeviation)
install(TARGETS runStandardDeviation RUNTIME DESTINATION bin)
