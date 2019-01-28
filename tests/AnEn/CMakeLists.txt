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

if(${ENABLE_MPI})
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
    target_link_libraries(testAnEn ${MPI_CXX_LIBRARIES})
    if(MPI_CXX_COMPILE_FLAGS)
        set_target_properties(testAnEn PROPERTIES
            COMPILE_FLAGS ${MPI_CXX_COMPILE_FLAGS})
    endif(MPI_CXX_COMPILE_FLAGS)
    if(MPI_CXX_LINK_FLAGS)
        set_target_properties(testAnEn PROPERTIES
            LINK_FLAGS ${MPI_CXX_LINK_FLAGS})
    endif(MPI_CXX_LINK_FLAGS)
endif(${ENABLE_MPI}) 

set_target_properties(runAnEn PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runAnEn)
install(TARGETS runAnEn RUNTIME DESTINATION bin)
