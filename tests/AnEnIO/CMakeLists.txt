cmake_minimum_required(VERSION 2.8)
project(TestAnEnIO)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testAnEnIO "testAnEnIO.cpp")
add_executable(runAnEnIO "runnerAnEnIO.cpp")

add_dependencies(testAnEnIO AnEnIO AnEn)
add_dependencies(runAnEnIO AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testAnEnIO ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runAnEnIO ${UnitTestLibs} testAnEnIO AnEnIO AnEn)
else(CPPUNIT_FOUND)
    add_dependencies(testAnEnIO cppunit)
    add_dependencies(runAnEnIO cppunit)

    target_link_libraries (testAnEnIO cppunit AnEnIO AnEn)
    target_link_libraries (runAnEnIO cppunit testAnEnIO AnEnIO AnEn)
endif(CPPUNIT_FOUND)

if(${ENABLE_MPI})
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
    target_link_libraries(testAnEnIO ${MPI_CXX_LIBRARIES})
    if(MPI_CXX_COMPILE_FLAGS)
        set_target_properties(testAnEnIO PROPERTIES
            COMPILE_FLAGS ${MPI_CXX_COMPILE_FLAGS})
    endif(MPI_CXX_COMPILE_FLAGS)
    if(MPI_CXX_LINK_FLAGS)
        set_target_properties(testAnEnIO PROPERTIES
            LINK_FLAGS ${MPI_CXX_LINK_FLAGS})
    endif(MPI_CXX_LINK_FLAGS)
endif(${ENABLE_MPI}) 

add_definitions(-D_PATH_OBSERVATIONS="${CMAKE_SOURCE_DIR}/tests/Data/test_observations.nc"
    -D_PATH_FORECASTS="${CMAKE_SOURCE_DIR}/tests/Data/test_forecasts.nc")

set_target_properties(runAnEnIO PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runAnEnIO)
install(TARGETS runAnEnIO RUNTIME DESTINATION bin)
