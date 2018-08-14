cmake_minimum_required(VERSION 2.8)
project(TestSimilarityMatrices)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(testSimilarityMatrices "testSimilarityMatrices.cpp")
add_executable(runSimilarityMatrices "runnerSimilarityMatrices.cpp")

add_dependencies(testSimilarityMatrices AnEnIO AnEn)
add_dependencies(runSimilarityMatrices AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testSimilarityMatrices ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runSimilarityMatrices ${UnitTestLibs} testSimilarityMatrices AnEnIO AnEn)
else(CPPUNIT_FOUND)
add_dependencies(testSimilarityMatrices cppunit)
add_dependencies(runSimilarityMatrices cppunit)

target_link_libraries (testSimilarityMatrices cppunit AnEnIO AnEn)
target_link_libraries (runSimilarityMatrices cppunit testSimilarityMatrices AnEnIO AnEn)
endif(CPPUNIT_FOUND)

set_target_properties(runSimilarityMatrices PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runSimilarityMatrices)
