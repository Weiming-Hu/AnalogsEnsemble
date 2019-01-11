cmake_minimum_required(VERSION 2.8)
project(TestGribConverter)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/apps/app_gribConverter")

add_library(testGribConverter "testGribConverter.cpp;${CMAKE_SOURCE_DIR}/apps/app_gribConverter/gribConverterFunctions.cpp")
add_executable(runGribConverter "runnerGribConverter.cpp")

#add_dependencies(testGribConverter AnEnIO AnEn)
#add_dependencies(runGribConverter AnEnIO AnEn)

if (CPPUNIT_FOUND)
    target_link_libraries (testGribConverter ${UnitTestLibs} AnEnIO AnEn)
    target_link_libraries (runGribConverter ${UnitTestLibs} testGribConverter AnEnIO AnEn)
else(CPPUNIT_FOUND)
    add_dependencies(testGribConverter cppunit)
    add_dependencies(runGribConverter cppunit)

    target_link_libraries (testGribConverter cppunit AnEnIO AnEn)
    target_link_libraries (runGribConverter cppunit testGribConverter AnEnIO AnEn)
endif(CPPUNIT_FOUND)


if (ECCODES_TYPE STREQUAL "SYSTEM")
    find_package(eccodes REQUIRED)
    include_directories(${ECCODES_INCLUDE_DIRS})
    target_link_libraries(testGribConverter eccodes)
    target_link_libraries (runGribConverter eccodes)
else (ECCODES_TYPE STREQUAL "SYSTEM")
    include_directories(${ECCODES_INCLUDE_DIRS})
    ExternalProject_Get_Property(ECCODES_PROJECT install_dir)
    add_library(eccodes_built SHARED IMPORTED)
    set_target_properties(eccodes_built PROPERTIES
        IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}eccodes${CMAKE_SHARED_LIBRARY_SUFFIX}")
    add_dependencies(eccodes_built ECCODES_PROJECT)
    target_link_libraries(testGribConverter eccodes_built)
    target_link_libraries (runGribConverter eccodes_built)

endif (ECCODES_TYPE STREQUAL "SYSTEM")

if(BOOST_TYPE STREQUAL "BUILD")
    target_link_libraries (testGribConverter datetime)
endif(BOOST_TYPE STREQUAL "BUILD")

add_definitions(-D_PATH_GRIB1="${CMAKE_SOURCE_DIR}/tests/Data/test_multi_fields.grib2")

set_target_properties(runGribConverter PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
add_test(NAME ${PROJECT_NAME} COMMAND runGribConverter)
install(TARGETS runGribConverter RUNTIME DESTINATION bin)
