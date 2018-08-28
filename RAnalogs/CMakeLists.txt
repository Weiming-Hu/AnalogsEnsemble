cmake_minimum_required (VERSION 2.8)
project (RAnalogs LANGUAGES NONE)

include(ExternalProject)
set(LIBRARY_NAME "RAnEn")

set(LIBRARY_VERSION "3.0.0")
message(STATUS "RAnEn version ${LIBRARY_VERSION}")

set(RAnEn_PACKAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_NAME}")
set(RAnEn_SOURCE_DIR "${RAnEn_PACKAGE_DIR}/src")

set(RAnEn_EXTRA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/CAnEn/include")
set(RAnEn_EXTRA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/CAnEn/src")

# remove old file if it exists
set(OLD_RAnEn_TARFILE "${CMAKE_CURRENT_SOURCE_DIR}/precompiled/${LIBRARY_NAME}_${LIBRARY_VERSION}.tar.gz")

if(EXISTS ${OLD_RAnEn_TARFILE})
    file(REMOVE ${OLD_RAnEn_TARFILE})
endif(EXISTS ${OLD_RAnEn_TARFILE})

##########################
# complete src directory #
##########################
# because some of the source files are deliberately missing in the src directory
# we will copy them from the original directory every time we build our R package.
#
# By doing this, we make sure
#
# 1) we don't maintain the same code at two different places;
# 2) we always build the R package using the latest C++ code;
#
# make sure all the required source and header files exist
if(NOT EXISTS ${RAnEn_SOURCE_DIR})
    message(FATAL_ERROR "ERROR: source file directory for RAnEn package could not be found in ${RAnEn_SOURCE_DIR}")
endif(NOT EXISTS ${RAnEn_SOURCE_DIR})

if(NOT EXISTS ${RAnEn_EXTRA_SOURCE_DIR})
    message(FATAL_ERROR "ERROR: extra source file directory for RAnEn package could not be found in ${RAnEn_EXTRA_SOURCE_DIR}")
endif(NOT EXISTS ${RAnEn_EXTRA_SOURCE_DIR})

if(NOT EXISTS ${RAnEn_EXTRA_INCLUDE_DIR})
    message(FATAL_ERROR "ERROR: extra include file directory for RAnEn package could not be found in ${RAnEn_EXTRA_INCLUDE_DIR}")
endif(NOT EXISTS ${RAnEn_EXTRA_INCLUDE_DIR})

# copy files
set(REQUIRED_FILE_NAME "Analogs;AnEn;Array4D;Forecasts;Observations;Parameters;SimilarityMatrices;StandardDeviation;Stations;Times")

foreach(file_name ${REQUIRED_FILE_NAME})
    file(COPY "${RAnEn_EXTRA_INCLUDE_DIR}/${file_name}.h"
        DESTINATION ${RAnEn_SOURCE_DIR})
    file(COPY "${RAnEn_EXTRA_SOURCE_DIR}/${file_name}.cpp"
        DESTINATION ${RAnEn_SOURCE_DIR})
endforeach(file_name ${REQUIRED_FILE_NAME})

file(COPY "${RAnEn_EXTRA_INCLUDE_DIR}/colorTexts.h" DESTINATION ${RAnEn_SOURCE_DIR})

####################################
# configure files in RAnEn package #
####################################
# build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set(CXX_FLAGS_RELEASE "-Wno-unused-variable -Wno-unused-but-set-variable -Wno-deprecated -Wno-sign-compare -Wno-unused-local-typedefs")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS_RELEASE}")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

# write the configure files to RAnEn package
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/configure_files/DESCRIPTION"
    "${RAnEn_PACKAGE_DIR}/DESCRIPTION")
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

    # I didn't have an easy time using configure script on Windows
    # so I decided not to use any script on Windows, and set the flags with assumptions
    #
    message(STATUS "This is a Windows. No configure files will be used. Use the default Rcpp build system.")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/configure_files/Makevars.win.in"
        "${RAnEn_SOURCE_DIR}/Makevars.win")

else(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(OPENMP_CXXFLAGS "@OPENMP_CXXFLAGS@")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/configure_files/Makevars.in"
        "${RAnEn_SOURCE_DIR}/Makevars.in" ESCAPE_QUOTES)

    message(STATUS "The system is ${CMAKE_SYSTEM_NAME}")
    message(STATUS "Add additional configure files for Rcpp build system.")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/configure_files/configure" DESTINATION ${RAnEn_PACKAGE_DIR})
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/configure_files/configure.ac" DESTINATION ${RAnEn_PACKAGE_DIR})
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/configure_files/aclocal.m4" DESTINATION ${RAnEn_PACKAGE_DIR})

endif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

##########################
# customize clean target #
##########################
#
add_subdirectory(${RAnEn_SOURCE_DIR})

set(FILES_TO_DELETE "")
foreach(file ${REQUIRED_FILE_NAME})
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file}.h")
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file}.cpp")
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file}.o")
endforeach(file ${REQUIRED_FILE_NAME})
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/Makevars.in")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/colorTexts.h")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/RAnEn.so")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/RcppExports.o")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/Rinterface.o")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_PACKAGE_DIR}/DESCRIPTION")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_PACKAGE_DIR}/configure")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_PACKAGE_DIR}/configure.ac")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_PACKAGE_DIR}/aclocal.m4")

# I decide to keep this tarball file because it might be useful for installing 
# the package on different platforms
#
#set(FILES_TO_DELETE "${FILES_TO_DELETE};${CMAKE_CURRENT_SOURCE_DIR}/precompiled/${LIBRARY_NAME}_${LIBRARY_VERSION}.tar.gz")

message(STATUS "Add clean-rsrc target for cleaning the copied files")
add_custom_target(
    clean-rsrc 
    COMMAND ${CMAKE_COMMAND} -E remove ${FILES_TO_DELETE}
    COMMENT "clean the copied files in RAnEn/src. You will need to cmake again if you want to make.")

option(CONFIGURE_ONLY "Only configure the R installation. Manual installation needed." OFF)
if(CONFIGURE_ONLY)
    message(STATUS "Only RAnEn configuration is done! No target has been built.")
    return()
endif(CONFIGURE_ONLY)

####################
# define functions #
####################
#
# This function is shared by [@Florian](https://stackoverflow.com/questions/44747896/is-it-possible-to-find-all-the-executables-with-the-same-name/44750064#44750064)
#
if(NOT R_COMMAND)
    message(STATUS "Not specifying the R_COMMAND. Before I try to find it myself, check your CMake version to see whether it supports the function to be defined.")
    if (${CMAKE_MAJOR_VERSION} GREATER 2 AND ${CMAKE_MINOR_VERSION} GREATER 2)
        cmake_policy(SET CMP0057 NEW)
        function(find_program_all _var)
            if (NOT DEFINED ${_var})
                while(1)
                    unset(_found CACHE)
                    find_program(_found ${ARGN})
                    if (_found AND NOT _found IN_LIST ${_var})
                        set(${_var} "${${_var}};${_found}" CACHE FILEPATH "Path to a program." FORCE)
                        # ignore with the next try
                        get_filename_component(_dir "${_found}" DIRECTORY)
                        list(APPEND CMAKE_IGNORE_PATH "${_dir}")
                    else()
                        unset(_found CACHE)
                        break()
                    endif()
                endwhile()
            endif()
        endfunction()
    else(${CMAKE_MAJOR_VERSION} GREATER 2 AND ${CMAKE_MINOR_VERSION} GREATER 2)
        message(FATAL_ERROR "CMake version should be higher than 3.3 in order for me to check if multiple Rs exist. Please specify R_COMMAND to cmake.")
    endif(${CMAKE_MAJOR_VERSION} GREATER 2 AND ${CMAKE_MINOR_VERSION} GREATER 2)
endif(NOT R_COMMAND)


#########################
# find the R executable #
#########################
#
# because we need this to install the package
#
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
if(NOT R_COMMAND)
    find_program_all(R_COMMAND "R")
    list(LENGTH R_COMMAND R_COMMAND_length)

    if(R_COMMAND_length STREQUAL 0)
        message(FATAL_ERROR "ERROR: could not find R executable.")
    elseif(R_COMMAND_length STREQUAL 1)
        message(STATUS "R executable is found at ${R_COMMAND}")
    else(R_COMMAND_length STREQUAL 0)
        message(FATAL_ERROR "ERROR: Multiple R executables are found. Specify the one using by cmake -DR_COMMAND=[R executable] ..")
    endif(R_COMMAND_length STREQUAL 0)

else(NOT R_COMMAND)
    message(STATUS "R_COMMAND is set to ${R_COMMAND}")
endif(NOT R_COMMAND)

add_custom_target(
    build-package ALL ${R_COMMAND} CMD build ${RAnEn_PACKAGE_DIR}
    COMMENT "target for building RAnEn package")
add_custom_target(
    copy-tarball ALL ${CMAKE_COMMAND} -E copy ${LIBRARY_NAME}_${LIBRARY_VERSION}.tar.gz ${CMAKE_CURRENT_SOURCE_DIR}/precompiled/${LIBRARY_NAME}_${LIBRARY_VERSION}.tar.gz
    COMMENT "target for copying package tarball")
add_custom_target(
    install-package ALL ${R_COMMAND} CMD INSTALL "${LIBRARY_NAME}_${LIBRARY_VERSION}.tar.gz"
    COMMENT "target for installing RAnEn package")

#add_dependencies(clean-rsrc install-package)
add_dependencies(install-package copy-tarball)
add_dependencies(copy-tarball build-package)
