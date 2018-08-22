cmake_minimum_required(VERSION 2.8)
project(ParallelEnsembleForecasts)

enable_language(CXX C)
option(VERBOSE "verbose standout output" OFF)
option(CMAKE_BUILD_TESTS "Build tests" OFF)

set(IMPORTANT_PATH "${CMAKE_SOURCE_DIR}/important")
set(EXTRA_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${EXTRA_MODULE_PATH})
set(DEPENDENCY_PATH "${CMAKE_SOURCE_DIR}/dependency")

if(NOT EXISTS ${DEPENDENCY_PATH})
    file(MAKE_DIRECTORY ${DEPENDENCY_PATH})
endif(NOT EXISTS ${DEPENDENCY_PATH})
if(NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif(NOT CMAKE_INSTALL_LIBDIR)

set(COMMON_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/output")
if (EXISTS ${COMMON_OUTPUT_DIR})
    file(REMOVE_RECURSE ${COMMON_OUTPUT_DIR})
endif(EXISTS ${COMMON_OUTPUT_DIR})

# policy setting for backward compatibility
if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif(POLICY CMP0054)

# set this variable to prevent CMake from removing
# non-standard build paths. This would be helpful on
# supercomputers where modules are always loaded, rather
# than installed at the standard location.
#
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# if INSTALL_RAnEn is set to TRUE
# only install RAnEn package
#
option(INSTALL_RAnEn "install RAnEn package" OFF)
if(INSTALL_RAnEn)
    message(STATUS "install RAnEn package")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/RAnalogs")
    return()
endif(INSTALL_RAnEn)


###################
# build libraries #
###################
# thrid packages required by CAnEn and CAnEnIO
option(ENABLE_MPI "enable suppor for MPI" OFF)
message(STATUS "ENABLE_MPI is set to ${ENABLE_MPI}")

if(ENABLE_MPI)
    add_definitions(-DENABLE_MPI)
endif(ENABLE_MPI)

file(MAKE_DIRECTORY ${COMMON_OUTPUT_DIR})
file(MAKE_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")

include(CMakeLists_netCDF.txt)
include(CMakeLists_Boost.txt)

# check if the compiler is GCC
if (NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
    # reference for message
    # https://cmake.org/cmake/help/v2.8.8/cmake.html#command%3amessage
    #
    message (WARNING "It is suggested to compile this project using GCC (version >= 4.9)!
    You are using ${CMAKE_CXX_COMPILER_ID}.
    SUGGESTION: specify GCC using CC and CXX. CC=[C compiler] CXX=[CXX compiler] cmake ..")
endif (NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))

# check the version of the compiler if GCC is used
if (${CMAKE_COMPILER_IS_GNUCXX})
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
        message(FATAL_ERROR "ERROR: CAnEn requires at least GCC version 4.9 to fully support c++11 standard! 
        SUGGESTION: specify another GCC using CC and CXX. CC=[C compiler] CXX=[CXX compiler] cmake ..")
    endif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
endif (${CMAKE_COMPILER_IS_GNUCXX})

# set c++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# compiler flags for Debug and Release
set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
set (CMAKE_CXX_FLAGS_RELEASE "-g -O3")
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)
message (STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# find OpenMP
find_package (OpenMP)
if (${OPENMP_FOUND})
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else (${OPENMP_FOUND})
    message (WARNING "No OpenMP support found for the compiler: ${CMAKE_CXX_COMPILER}
    The process goes on, but the executable won't support multi-threading.
    SUGGESTION: specify another compiler using CC and CXX. CC=[C compiler] CXX=[CXX compiler] cmake ..")
endif (${OPENMP_FOUND})

message(STATUS "configure CAnEnIO library")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/CAnEnIO")

# install CAnEn library
message(STATUS "configure CAnEn library")
add_subdirectory("${CMAKE_SOURCE_DIR}/CAnEn")


#####################
# build executables #
#####################
add_subdirectory("${CMAKE_SOURCE_DIR}/app_analogsGenerator")
#add_subdirectory("${CMAKE_SOURCE_DIR}/app_canalogs")
#add_subdirectory("${CMAKE_SOURCE_DIR}/app_CAnEnIO")
#add_subdirectory("${CMAKE_SOURCE_DIR}/app_data_manager")


#####################
#   Build tests     #
#####################
# if CMAKE_BUILD_TESTS is set to ON, build the test cases
if(CMAKE_BUILD_TESTS)
    message(STATUS "Build various tests")
    file(MAKE_DIRECTORY "${COMMON_OUTPUT_DIR}/test")
    enable_testing()
    add_custom_target(test_all ${CMAKE_CTEST_COMMAND} -V)

    if (NOT CPPUNIT_TYPE)
        set (CPPUNIT_TYPE "BUILD" CACHE STRING
            "The type of CPPUNIT library that is going to use by the make process. Should be either SYSTEM or BUILD.")
    endif (NOT CPPUNIT_TYPE)
    message (STATUS "CPPUNIT_TYPE is set to ${CPPUNIT_TYPE}")

    if (CPPUNIT_TYPE STREQUAL "SYSTEM")
        # Find CppUnit
        find_package(CppUnit)
    else(CPPUNIT_TYPE STREQUAL "SYSTEM")
        set(CPPUNIT_FOUND FALSE)
    endif(CPPUNIT_TYPE STREQUAL "SYSTEM")

    if (CPPUNIT_FOUND)
        message(STATUS "CppUnit include files found at ${CPPUNIT_INCLUDE_DIR}")
        message(STATUS "CppUnit lib files found at ${CPPUNIT_LIBRARIES}")
        message(WARNING "This might create problems if you are using a different compiler when you compiled CppUnit. Try to set CPPUNIT_TYPE to BUILD to let CMake build it.")

        include_directories(${CPPUNIT_INCLUDE_DIR})
        LIST(APPEND UnitTestLibs ${CPPUNIT_LIBRARIES})
    else(CPPUNIT_FOUND)
        message(STATUS "CppUnit not found. Build it now.")
        find_package(Git REQUIRED)

        set (LIBRARY_CPPUNIT "${DEPENDENCY_PATH}/cppunit")
        if (NOT EXISTS ${LIBRARY_CPPUNIT})
            execute_process(COMMAND ${GIT_EXECUTABLE} "clone" "https://github.com/Weiming-Hu/CppUnit.git" ${LIBRARY_CPPUNIT})
        else(NOT EXISTS ${LIBRARY_CPPUNIT})
            message(STATUS "CppUnit has already been downloaded!")
        endif(NOT EXISTS ${LIBRARY_CPPUNIT})

        include_directories("${LIBRARY_CPPUNIT}/include")
        add_subdirectory("${LIBRARY_CPPUNIT}")
    endif(CPPUNIT_FOUND)

    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")

else(CMAKE_BUILD_TESTS)
    message(STATUS "By default, no tests built (CMAKE_BUILD_TESTS OFF)")
endif(CMAKE_BUILD_TESTS)
