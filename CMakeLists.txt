cmake_minimum_required(VERSION 2.8)
project(ParallelEnsembleForecasts)

# policy setting for backward compatibility
if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif(POLICY CMP0054)

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif(POLICY CMP0042)
set(CMAKE_MACOSX_RPATH TRUE)

enable_language(CXX C)
option(VERBOSE "verbose standout output" OFF)
option(CMAKE_BUILD_TESTS "Build tests" OFF)

set(IMPORTANT_PATH "${CMAKE_SOURCE_DIR}/important")
set(EXTRA_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
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

# set this variable to prevent CMake from removing
# non-standard build paths. This would be helpful on
# supercomputers where modules are always loaded, rather
# than installed at the standard location.
#
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# This variable is used for all the external projects
# to deal with RPATH issues
#
set(RPATH_ARGS "-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=${CMAKE_INSTALL_RPATH_USE_LINK_PATH}")
set(RPATH_ARGS ${RPATH_ARGS} "-DCMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH}")
set(RPATH_ARGS ${RPATH_ARGS} "-DCMAKE_MACOSX_RPATH=${CMAKE_MACOSX_RPATH}")


###################
# build libraries #
###################
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

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# compiler flags for Debug and Release
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)
message (STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
set (CMAKE_CXX_FLAGS_RELEASE "-O2 -w")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# find OpenMP
find_package (OpenMP)
if (${OPENMP_FOUND})
    message(STATUS "Using OpenMP ${OpenMP_CXX_FLAGS}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else (${OPENMP_FOUND})
    message (WARNING "No OpenMP support found for the compiler: ${CMAKE_CXX_COMPILER}
    The process goes on, but the executable won't support multi-threading.
    SUGGESTION: specify another compiler using CC and CXX. CC=[C compiler] CXX=[CXX compiler] cmake ..")
endif (${OPENMP_FOUND})

# if INSTALL_RAnEn is set to TRUE
# only install RAnEn package
#
option(INSTALL_RAnEn "install RAnEn package" OFF)
if(INSTALL_RAnEn)
    message(STATUS "install RAnEn package")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/RAnalogs")
    return()
else (INSTALL_RAnEn)
	# The Release and Debug mode also apply to the libraries that we are going to build
	# But we don't want the -fopenmp option to be applied to dependent libraries
	#
	include(CMakeLists_netCDF.txt)
	include(CMakeLists_Boost.txt)
endif(INSTALL_RAnEn)

# thrid packages required by CAnEn and CAnEnIO
option(ENABLE_MPI "enable suppor for MPI" OFF)
message(STATUS "ENABLE_MPI is set to ${ENABLE_MPI}")

if(ENABLE_MPI)
    add_definitions(-DENABLE_MPI)
endif(ENABLE_MPI)

file(MAKE_DIRECTORY ${COMMON_OUTPUT_DIR})
file(MAKE_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")
file(MAKE_DIRECTORY "${COMMON_OUTPUT_DIR}/lib")

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
        set (CPPUNIT_TYPE "BUILD")
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
        include(ExternalProject)

        set(CMAKE_EXTERNAL_PROJECT_BUILD_TYPE "Release")
        set(CPPUNIT_ROOT_PATH ${DEPENDENCY_PATH})
        set(CPPUNIT_INSTALL_PREFIX "${DEPENDENCY_PATH}/install")
        set(CPPUNIT_URL "https://github.com/Weiming-Hu/CppUnit.git")
        set(CPPUNIT_CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CPPUNIT_INSTALL_PREFIX}")

        string(REPLACE ";" "|" CPPUNIT_ALT_CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}")
        set(CPPUNIT_PREFIX_PATH "-DCMAKE_PREFIX_PATH=${CPPUNIT_ALT_CMAKE_PREFIX_PATH}")
        set(CPPUNIT_CMAKE_ARGS ${CPPUNIT_CMAKE_ARGS} ${CPPUNIT_PREFIX_PATH} ${RPATH_ARGS})

        if(VERBOSE)
            # If verbose is set to true, the following information will be printed
            set(VERBOSE_DOWNLOAD 0)
            set(VERBOSE_CONFIGURE 0)
            set(VERBOSE_BUILD 0)
            set(VERBOSE_INSTALL 0) 
            set(VERBOSE_TEST 0)
        else(VERBOSE)
            # If verbose is set to true, the following information will be logged to file
            set(VERBOSE_DOWNLOAD 1)
            set(VERBOSE_CONFIGURE 1)
            set(VERBOSE_BUILD 1)
            set(VERBOSE_INSTALL 1) 
            set(VERBOSE_TEST 1)
        endif(VERBOSE)

        ExternalProject_add(CPPUNIT_PROJECT
            PREFIX ${CPPUNIT_ROOT_PATH}
            GIT_REPOSITORY ${CPPUNIT_URL}
            GIT_TAG master
            PATCH_COMMAND ""
            UPDATE_COMMAND ""
            LIST_SEPARATOR | # Use the alternate list separator
            CMAKE_ARGS ${CPPUNIT_CMAKE_ARGS}
            BUILD_COMMAND ${MAKE}
            LOG_DOWNLOAD ${VERBOSE_DOWNLOAD}
            LOG_CONFIGURE ${VERBOSE_CONFIGURE}
            LOG_BUILD ${VERBOSE_BUILD}
            LOG_TEST ${VERBOSE_TEST}
            LOG_INSTALL ${VERBOSE_INSTALL})

        ExternalProject_Get_Property(CPPUNIT_PROJECT install_dir)
        add_library(cppunit STATIC IMPORTED)
        set_target_properties(cppunit
            PROPERTIES IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}cppunit${CMAKE_STATIC_LIBRARY_SUFFIX}")
        include_directories("${CPPUNIT_INSTALL_PREFIX}/include")

    endif(CPPUNIT_FOUND)

    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")

else(CMAKE_BUILD_TESTS)
    message(STATUS "By default, no tests built (CMAKE_BUILD_TESTS OFF)")
endif(CMAKE_BUILD_TESTS)


#########################
#      Add Uninstall    #
#########################
# uninstall target
if(NOT TARGET uninstall)
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/CMakeLists_Uninstall.txt)
endif(NOT TARGET uninstall)

if (HDF5_BUILT)
    message(WARNING "HDF5 is built. Please keep this build directory becuase the program needs it.")
endif(HDF5_BUILT)

if(NETCDF_BUILT)
    message(WARNING "netCDF is built. Please keep this build directory becuase the program needs it.")
endif(NETCDF_BUILT)
