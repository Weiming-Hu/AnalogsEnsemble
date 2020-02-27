###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This file includes some macros for the project
#

# Define a macro for find_package so that it works for our subprojects.
# If the package is in the list SUBPROJECT_TARGETS, this macro will simply 
# make the find_package into an no-op.
# 
# ${ARGV0} is the name of the target.
#
macro(find_package)

    if(NOT "${ARGV0}" IN_LIST SUBPROJECT_TARGETS)
        _find_package(${ARGV})
    endif(NOT "${ARGV0}" IN_LIST SUBPROJECT_TARGETS)

endmacro(find_package)


# Add a folder as a test target. This macro takes a single argument which is the
# class name to be tested. This name should also be the folder name. For example,
# if the class name is Times, the folder name should also be Times, the source
# file name is assumed to be testTimes.cpp, and the executable source file is assumed
# to be runnerTimes.cpp.
#
macro(PAnEn_test_this)

    add_library("test${ARGV0}" "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/test${ARGV0}.cpp")
    target_link_libraries("test${ARGV0}" PUBLIC AnEnIO ${CPPUNIT_LIBRARIES})
    target_include_directories("test${ARGV0}"
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}>)

    add_executable("run${ARGV0}" "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}/runner${ARGV0}.cpp")
    target_link_libraries("run${ARGV0}" "test${ARGV0}")

    add_test(NAME Test${ARGV0} COMMAND "run${ARGV0}")
    install(TARGETS "run${ARGV0}" RUNTIME DESTINATION test)

endmacro(PAnEn_test_this)


# Check whether boost source files have been downloaded
macro(check_boost_download)

    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/boost-cmake/CMakeLists.txt)
        find_package(Git 1.9 REQUIRED QUIET)
        message(STATUS "Download source files for Boost")
        execute_process(COMMAND ${GIT_EXECUTABLE} "clone"
            "https://github.com/Orphis/boost-cmake.git"
            "${CMAKE_CURRENT_BINARY_DIR}/boost-cmake")
    endif(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/boost-cmake/CMakeLists.txt)

endmacro(check_boost_download)


# Check whether netcdf cxx4 source files have been downloaded
macro(check_netcdfcxx_download)

    if(NOT EXISTS ${NETCDFCXX_ROOT}/CMakeLists.txt)

        if(NOT EXISTS NETCDFCXX_TARBALL)
            message(STATUS "Download the tarball for NetCDF C++4 APIs")
            file(DOWNLOAD "https://github.com/Unidata/netcdf-cxx4/archive/v4.3.0.tar.gz" ${NETCDFCXX_TARBALL})
        else(NOT EXISTS NETCDFCXX_TARBALL)
            message(STATUS "NetCDF C++4 tarball exists.")
        endif(NOT EXISTS NETCDFCXX_TARBALL)

        if(NOT EXISTS ${NETCDFCXX_ROOT})
            message(STATUS "Extracting source files for NetCDF C++4 APIs")
            execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xzf ${NETCDFCXX_TARBALL}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        else(NOT EXISTS ${NETCDFCXX_ROOT})
            message(STATUS "NetCDF C++4 root folder exists.")
        endif(NOT EXISTS ${NETCDFCXX_ROOT})

    endif(NOT EXISTS ${NETCDFCXX_ROOT}/CMakeLists.txt)

endmacro(check_netcdfcxx_download)
