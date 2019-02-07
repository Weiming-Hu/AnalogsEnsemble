cmake_minimum_required(VERSION 2.8)
project(TestFunctions)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testFunctions "testFunctions.cpp")
add_executable(runFunctions "runnerFunctions.cpp")

add_dependencies(testFunctions AnEnIO AnEn)
add_dependencies(runFunctions AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testFunctions ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runFunctions ${UnitTestLibs} testFunctions AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testFunctions cppunit)
add_dependencies(runFunctions cppunit)

target_link_libraries (testFunctions cppunit AnEnIO AnEn)
target_link_libraries (runFunctions cppunit testFunctions AnEnIO AnEn)
endif(CPPUNIT_FOUND)

if(${ENABLE_MPI})
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
    target_link_libraries(testFunctions ${MPI_CXX_LIBRARIES})
    if(MPI_CXX_COMPILE_FLAGS)
        set_target_properties(testFunctions PROPERTIES
            COMPILE_FLAGS ${MPI_CXX_COMPILE_FLAGS})
    endif(MPI_CXX_COMPILE_FLAGS)
    if(MPI_CXX_LINK_FLAGS)
        set_target_properties(testFunctions PROPERTIES
            LINK_FLAGS ${MPI_CXX_LINK_FLAGS})
    endif(MPI_CXX_LINK_FLAGS)
endif(${ENABLE_MPI}) 

set_target_properties(runFunctions PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runFunctions)
install(TARGETS runFunctions RUNTIME DESTINATION bin)
