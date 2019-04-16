cmake_minimum_required(VERSION 2.8)
project(TestStation)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testStation "testStation.cpp")
add_executable(runStation "runnerStation.cpp")

add_dependencies(testStation AnEnIO AnEn)
add_dependencies(runStation AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testStation ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runStation ${UnitTestLibs} testStation AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testStation cppunit)
add_dependencies(runStation cppunit)

target_link_libraries (testStation cppunit AnEnIO AnEn)
target_link_libraries (runStation cppunit testStation AnEnIO AnEn)
endif(CPPUNIT_FOUND)

if(${ENABLE_MPI})
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
    target_link_libraries(testStation ${MPI_CXX_LIBRARIES})
    if(MPI_CXX_COMPILE_FLAGS)
        set_target_properties(testStation PROPERTIES
            COMPILE_FLAGS ${MPI_CXX_COMPILE_FLAGS})
    endif(MPI_CXX_COMPILE_FLAGS)
    if(MPI_CXX_LINK_FLAGS)
        set_target_properties(testStation PROPERTIES
            LINK_FLAGS ${MPI_CXX_LINK_FLAGS})
    endif(MPI_CXX_LINK_FLAGS)
endif(${ENABLE_MPI}) 

set_target_properties(runStation PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runStation)
install(TARGETS runStation RUNTIME DESTINATION bin)
