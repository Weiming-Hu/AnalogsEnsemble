cmake_minimum_required(VERSION 2.8)
project(TestGribConverter)
enable_language(CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/apps/app_gribConverter")

add_library(testGribConverter "testGribConverter.cpp;${CMAKE_SOURCE_DIR}/apps/app_gribConverter/gribConverterFunctions.cpp")
add_executable(runGribConverter "runnerGribConverter.cpp")

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
    target_link_libraries(testGribConverter ${ECCODES_LIBRARIES})
    target_link_libraries(runGribConverter ${ECCODES_LIBRARIES})
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

if(${ENABLE_MPI})
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
    target_link_libraries(testGribConverter ${MPI_CXX_LIBRARIES})
    if(MPI_CXX_COMPILE_FLAGS)
        set_target_properties(testGribConverter PROPERTIES
            COMPILE_FLAGS ${MPI_CXX_COMPILE_FLAGS})
    endif(MPI_CXX_COMPILE_FLAGS)
    if(MPI_CXX_LINK_FLAGS)
        set_target_properties(testGribConverter PROPERTIES
            LINK_FLAGS ${MPI_CXX_LINK_FLAGS})
    endif(MPI_CXX_LINK_FLAGS)
endif(${ENABLE_MPI}) 

set_target_properties(runGribConverter PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/test")

# Add additional RPATH
if (ECCODES_TYPE STREQUAL "BUILD")
    set_target_properties(runGribConverter PROPERTIES 
        INSTALL_RPATH "${CMAKE_INSTALL_RPATH}"
        BUILD_WITH_INSTALL_RPATH ON)
endif(ECCODES_TYPE STREQUAL "BUILD")

add_test(NAME ${PROJECT_NAME} COMMAND runGribConverter)
install(TARGETS runGribConverter RUNTIME DESTINATION bin)
