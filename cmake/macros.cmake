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


# If the boost cmake folder is empty, it is probably not downloaded. I'm going to
# print some help messages on how to download.
#
macro(check_boost_cmake_download)

    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/boost-cmake/CMakeLists.txt)
        find_package(Git 1.9 REQUIRED QUIET)
        message(STATUS "Checkout the submodule boost-cmake")
        execute_process(COMMAND ${GIT_EXECUTABLE} "submodule" "update" "--init")
    endif(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/boost-cmake/CMakeLists.txt)

endmacro(check_boost_cmake_download)
