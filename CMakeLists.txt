cmake_minimum_required(VERSION 2.8)
project(ParallelEnsembleForecasts)

set(GRAND_VERSION "3.7.1")
message("You are building version ${GRAND_VERSION}")
add_definitions(-D_APPVERSION="v ${GRAND_VERSION}")

# policy setting for backward compatibility
if(POLICY CMP0054)
    cmake_policy(SET CMP0054 NEW)
endif(POLICY CMP0054)

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif(POLICY CMP0042)

enable_language(CXX C)

##################################################################################
#                 Start of the list of parameters                                #
##################################################################################
option(CMAKE_BUILD_TESTS "Build tests" ON)
option(VERBOSE "Verbose standout output" OFF)
option(ENABLE_MPI "enable support for MPI" OFF)
option(INSTALL_RAnEn "install RAnEn package" OFF)
option(USE_NCCONFIG "Use nc-config to determine cflags and libdirs." OFF)
option(CODE_PROFILING "Insert profiling code and print time information." OFF)

option(BUILD_FS "Build file slicer" ON)
option(BUILD_AGG "Build file aggregator" ON)
option(BUILD_ANENS "Build analog selector" ON)
option(BUILD_ANENGEN "Build analog generator" ON)
option(BUILD_SIMS "Build similarity calculator" ON)
option(BUILD_WIND "Build wind field calculator" ON)
option(BUILD_SD "Build standard deviation calculator" ON)
option(BUILD_F2O "Build forecast to observation utility" ON)
option(BUILD_GRIBCONVERTER "Build gribConverter utility. Eccodes required." OFF)

# set this variable to prevent CMake from removing
# non-standard build paths. This would be helpful on
# supercomputers where modules are always loaded, rather
# than installed at the standard location.
#
option(CMAKE_MACOSX_RPATH "Whether RPATH should be used on MacOS" ON)
option(CMAKE_INSTALL_RPATH_USE_LINK_PATH "Whether to use link path during installation" ON)
##################################################################################
#                   End of the list of parameters                                #
##################################################################################

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
#if (EXISTS ${COMMON_OUTPUT_DIR})
#    file(REMOVE_RECURSE ${COMMON_OUTPUT_DIR})
#endif(EXISTS ${COMMON_OUTPUT_DIR})

if (CMAKE_INSTALL_RPATH)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${DEPENDENCY_PATH}/install/lib/;${COMMON_OUTPUT_DIR}/lib")
else (CMAKE_INSTALL_RPATH)
    set(CMAKE_INSTALL_RPATH "${DEPENDENCY_PATH}/install/lib/;${COMMON_OUTPUT_DIR}/lib")
endif (CMAKE_INSTALL_RPATH)
message(STATUS "CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}")

# This variable is used for all the external projects
# to deal with RPATH issues
#
set(RPATH_ARGS "-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=${CMAKE_INSTALL_RPATH_USE_LINK_PATH}")
set(RPATH_ARGS ${RPATH_ARGS} "-DCMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH}")
set(RPATH_ARGS ${RPATH_ARGS} "-DCMAKE_MACOSX_RPATH=${CMAKE_MACOSX_RPATH}")

# Define copyright message
add_definitions(-D_COPYRIGHT_MSG="Geoinformatics and Earth Observation Laboratory @ Penn State")


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

else(NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))

    # Mac OS SDK conflicts with GNU compilers. This setting disables
    # adding SDK path during compilation.
    #
    message(STATUS "Remove system root when GNU is used.")
    set(CMAKE_OSX_SYSROOT "")

endif (NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))

# check the version of the compiler if GCC is used
if (${CMAKE_COMPILER_IS_GNUCXX})
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
        message(FATAL_ERROR "ERROR: CAnEn requires at least GCC version 4.9 to fully support c++11 standard! 
        SUGGESTION: specify another GCC using CC and CXX. CC=[C compiler] CXX=[CXX compiler] cmake ..")
    endif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
endif (${CMAKE_COMPILER_IS_GNUCXX})

# Adding requirement for C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compiler flags for Debug and Release
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)
message (STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# Extensive warnings for Debug mode
if (CMAKE_CXX_FLAGS_DEBUG)
    # -Wall print all regular warnings
    # -Wextra print extra warnings
    #
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")
else (CMAKE_CXX_FLAGS_DEBUG)
    set (CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra")
endif (CMAKE_CXX_FLAGS_DEBUG)

# Suppress warning for Release mode
if (CMAKE_CXX_FLAGS_RELEASE)
    # -s strip out the debugging symbols
    # -w suppress all compiler and linker warnings
    # 
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -w")
else (CMAKE_CXX_FLAGS_RELEASE)
    set (CMAKE_CXX_FLAGS_RELEASE "-s -w")
endif (CMAKE_CXX_FLAGS_RELEASE)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS}")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

message(STATUS "C++ compiler options are: ${CMAKE_CXX_FLAGS}")

# find OpenMP
find_package (OpenMP)
if (${OPENMP_FOUND})
    message(STATUS "Using OpenMP ${OpenMP_CXX_FLAGS}")
    set (CMAKE_C_FLAGS "${OpenMP_C_FLAGS} ${CMAKE_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
else (${OPENMP_FOUND})
    message (WARNING "No OpenMP support found for the compiler: ${CMAKE_CXX_COMPILER}
    The process goes on, but the executable won't support multi-threading.
    SUGGESTION: specify another compiler using CC and CXX. CC=[C compiler] CXX=[CXX compiler] cmake ..")
endif (${OPENMP_FOUND})

if (VERBOSE)
    message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endif (VERBOSE)

if(CODE_PROFILING)
    message(STATUS "Insert codes to profiling functions and print time information.")
    add_definitions(-D_CODE_PROFILING)
endif(CODE_PROFILING)

# if INSTALL_RAnEn is set to TRUE
# only install RAnEn package
#
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

message(STATUS "ENABLE_MPI is set to ${ENABLE_MPI}")

if(ENABLE_MPI)
    # check MPI compiler
    find_package(MPI REQUIRED COMPONENTS C CXX)
    if(CMAKE_C_COMPILER MATCHES "^.*mpi.*$" AND
            CMAKE_CXX_COMPILER MATCHES "^.*mpi.*$")
        message(STATUS "Configure support with MPI compiler")
    else(CMAKE_C_COMPILER MATCHES "^.*mpi.*$" AND
            CMAKE_CXX_COMPILER MATCHES "^.*mpi.*$")
        message(WARNING "Please specify CC and CXX to mpicc compiler/wrapper.")
    endif(CMAKE_C_COMPILER MATCHES "^.*mpi.*$" AND
        CMAKE_CXX_COMPILER MATCHES "^.*mpi.*$")

    add_definitions(-D_ENABLE_MPI)
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
if (BUILD_SD)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_standardDeviationCalculator")
endif (BUILD_SD)

if (BUILD_F2O)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_forecastsToObservations")
endif (BUILD_F2O)

if (BUILD_SIMS)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_similarityCalculator")
endif (BUILD_SIMS)

if (BUILD_WIND)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_windFieldCalculator")
endif (BUILD_WIND)

if (BUILD_ANENGEN)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_analogGenerator")
endif (BUILD_ANEN)

if (BUILD_ANENS)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_analogSelector")
endif (BUILD_ANENS)

if (BUILD_AGG)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_fileAggregate")
endif (BUILD_AGG)

if (BUILD_FS)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_fileSlice")
endif (BUILD_FS)

if(ENABLE_MPI)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_mpiAnEnIO")
endif(ENABLE_MPI)

if (BUILD_GRIBCONVERTER)
    add_subdirectory("${CMAKE_SOURCE_DIR}/apps/app_gribConverter")
endif (BUILD_GRIBCONVERTER)


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

        include_directories(SYSTEM ${CPPUNIT_INCLUDE_DIR})
        LIST(APPEND UnitTestLibs ${CPPUNIT_LIBRARIES})
    else(CPPUNIT_FOUND)
        message(STATUS "CppUnit not found. Build it now.")
        include(ExternalProject)

        # There are generally two ways to link to an external library that is not built in the system:
        # - 1. Include the subdirectory if a CMakeLists.txt is provided, and this CMakeLists.txt 
        #      supports being included as a subdirectory. With that being said, the CMakeLists.txt
        #      carefully uses variables like CMake_SOURCE_DIR since, as a subdirectory, CMAKE_SOURCE_DIR
        #      is differentiated from CMAKE_CURRENT_SOURCE_DIR.
        # - 2. Add an external project and import the library. This approach is architecture dependent in
        #      that the full path to the built library needs to be specified at compile time. NetCDF-CXX4
        #      uses this method to be included in the project. This method is useful when you are building
        #      dependencies of dependencies so that you don't need to re-write CMakeLists.txt for each
        #      submodule to include their submodules.
        #
        find_package(Git 1.9 REQUIRED QUIET)

        set(CMAKE_EXTERNAL_PROJECT_BUILD_TYPE "Release")
        set(CPPUNIT_URL "https://github.com/Weiming-Hu/CppUnit.git")
        set (LIBRARY_CPPUNIT "${DEPENDENCY_PATH}/CppUnit")

        if(NOT EXISTS ${LIBRARY_CPPUNIT})
            execute_process(COMMAND ${GIT_EXECUTABLE} "clone" ${CPPUNIT_URL} ${LIBRARY_CPPUNIT})
        else(NOT EXISTS ${LIBRARY_CPPUNIT})
            message(STATUS "CppUnit submodule is already downloaded")
        endif(NOT EXISTS ${LIBRARY_CPPUNIT})

        include_directories (SYSTEM "${LIBRARY_CPPUNIT}/include")
        add_subdirectory ("${DEPENDENCY_PATH}/CppUnit")

    endif(CPPUNIT_FOUND)

    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")

else(CMAKE_BUILD_TESTS)
    message(STATUS "No tests built (CMAKE_BUILD_TESTS OFF)")
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


#########################
#     Add ducoment      #
#########################
# Use doxygen to generate documents for C++
find_package (Doxygen)
if (DOXYGEN_FOUND)
    set (DOXYGEN_QUIET "NO")
    set (DOXYGEN_GENERATE_XML "NO")
    set (DOXYGEN_GENERATE_HTML YES)
    set (DOXYGEN_GENERATE_MAN YES)
    set (DOXYGEN_GENERATE_LATEX "NO")
    set (DOXYGEN_PROJECT_NAME "Analog Ensemble")
    set (DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set (DOXYGEN_MAINPAGE_MDFILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set (DOXYGEN_EXCLUDE_PATTERNS "*build* *output* *dependency* *nbproject* *RAnalogs* *important* *cmake*")

    if (${DOXYGEN_DOT_FOUND})
        set (DOXYGEN_HAVE_DOT YES)
    else (${DOXYGEN_DOT_FOUND}) 
        set (DOXYGEN_HAVE_DOT "NO")
    endif (${DOXYGEN_DOT_FOUND})

    configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/doxygen-config.in" "${CMAKE_CURRENT_BINARY_DIR}/doxygen-config")

    message (STATUS "Doxygen found. Use make document to generate package C++ documentation.")

    add_custom_target(document
        COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/doxygen-config"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generate help page using doxygen")
endif (DOXYGEN_FOUND)
