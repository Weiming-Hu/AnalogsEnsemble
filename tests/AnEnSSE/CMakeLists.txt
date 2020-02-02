cmake_minimum_required(VERSION 2.8)
project(TestAnEnSSE)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testAnEnSSE "testAnEnSSE.cpp")
add_executable(runAnEnSSE "runnerAnEnSSE.cpp")

add_dependencies(testAnEnSSE AnEnIO AnEn)
add_dependencies(runAnEnSSE AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testAnEnSSE ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runAnEnSSE ${UnitTestLibs} testAnEnSSE AnEnIO AnEn)
else(CPPUNIT_FOUND)
    add_dependencies(testAnEnSSE cppunit)
    add_dependencies(runAnEnSSE cppunit)

    target_link_libraries (testAnEnSSE cppunit AnEnIO AnEn)
    target_link_libraries (runAnEnSSE cppunit testAnEnSSE AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runAnEnSSE PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runAnEnSSE)
install(TARGETS runAnEnSSE RUNTIME DESTINATION bin)
