cmake_minimum_required(VERSION 3.1)
project(RAnEn)

# Specify where the shared library should be
# set(CMAKE_SHARED_LIBRARY_PREFIX "")

# Add the cmake module directory to the source
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Define build type if not defined
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

# Find R
if (APPLE)
    find_library(R_LIBRARY R REQUIRED)
else (APPLE)
    find_package(R REQUIRED)
endif (APPLE)

# Find Rscript
if (RSCRIPT_COMMAND)
    message(STATUS "Rscript is set to ${RSCRIPT_COMMAND}.")
else (RSCRIPT_COMMAND)
    string(REGEX MATCH "^(.*)\/" R_PATH ${R_COMMAND})
    if (WIN32)
        find_program(RSCRIPT_COMMAND "Rscript.exe" HINTS ${R_PATH})
    else (WIN32)
        find_program(RSCRIPT_COMMAND "Rscript" HINTS ${R_PATH})
    endif (WIN32)

    if (RSCRIPT_COMMAND STREQUAL "RSCRIPT_COMMAND-NOTFOUND")
        message(FATAL_ERROR "Rscript is not found in ${R_PATH}. Please use R CMD INSTALL utility.")
    else (RSCRIPT_COMMAND STREQUAL "RSCRIPT_COMMAND-NOTFOUND")
        message(STATUS "Rscript: ${RSCRIPT_COMMAND}.")
    endif (RSCRIPT_COMMAND STREQUAL "RSCRIPT_COMMAND-NOTFOUND")
endif (RSCRIPT_COMMAND)

# Use Rscript to find Cxx flags for Rcpp
execute_process(COMMAND ${RSCRIPT_COMMAND} -e "Rcpp:::CxxFlags()" OUTPUT_VARIABLE RCPP_CXXFLAGS)
message(STATUS "Rcpp CxxFlags: ${RCPP_CXXFLAGS}.")

# Find Rcpp path
# Because CMake does not support non-greedy mode
# I have to use index approach to get rid of the prefix option
#
string (FIND ${RCPP_CXXFLAGS} "\/" PATH_START_INDEX)
string (SUBSTRING ${RCPP_CXXFLAGS} ${PATH_START_INDEX} -1 RCPP_DIR)
string(REGEX REPLACE "^(.+)\/include$" "\\1" RCPP_DIR ${RCPP_DIR})
string(REGEX REPLACE "^(.+)\/Rcpp$" "\\1" R_LIBS_DIR ${RCPP_DIR})

if (NOT EXISTS ${RCPP_DIR})
    # Although RCPP_DIR should exist intuitively, this check is designed for unexpected 
    # return from the Rscript
    #
    message(FATAL_ERROR "${RCPP_DIR} does not exist.")
endif (NOT EXISTS ${RCPP_DIR})

# Find R library BH
set(R_BH_INCLUDE_DIR "${R_LIBS_DIR}/BH/include")
if (EXISTS ${R_BH_INCLUDE_DIR})
    message(STATUS "R library BH include: ${R_BH_INCLUDE_DIR}.")
else (EXISTS ${R_BH_INCLUDE_DIR})
    message(FATAL_ERROR "R library BH is not installed. ${R_BH_INCLUDE_DIR} missing.")
endif (EXISTS ${R_BH_INCLUDE_DIR})

# Add the shared libarry target
file (GLOB SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(RAnEn SHARED ${SourceFiles})

# Import Rcpp library
add_library(Rcpp SHARED IMPORTED)
set_property(TARGET Rcpp PROPERTY IMPORTED_LOCATION "${RCPP_DIR}/libs/Rcpp${CMAKE_SHARED_LIBRARY_SUFFIX}")

# Include and link libraries
set_property(TARGET RAnEn APPEND PROPERTY COMPILE_FLAGS ${RCPP_CXXFLAGS})
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/src" ${R_INCLUDE_DIR})
target_link_libraries(RAnEn Rcpp R)

if (R_BH_INCLUDE_DIR)
    include_directories (${R_BH_INCLUDE_DIR})
endif (R_BH_INCLUDE_DIR)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET RAnEn APPEND PROPERTY COMPILE_FLAGS ${CXX_FLAGS_RELEASE})
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

# Enable C++11
set_property(TARGET RAnEn PROPERTY CXX_STANDARD 11)
set_property(TARGET RAnEn PROPERTY CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    target_compile_options(RAnEn PUBLIC "-std=c++11")
endif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")

# Add target to copy files
add_custom_target(copyPackage ALL)	

# Add all man files
file (GLOB RDFILES "${CMAKE_CURRENT_SOURCE_DIR}/man/*.Rd")

# Add all R source files
file (GLOB RFILES "${CMAKE_CURRENT_SOURCE_DIR}/R/*.R")

# Add all data files
file (GLOB RDATAFILES "${CMAKE_CURRENT_SOURCE_DIR}/data/*.RData")

# Configure DESCRIPTION
add_custom_command(TARGET copyPackage
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION.in" "${CMAKE_CURRENT_BINARY_DIR}/DESCRIPTION")

# Copy R files
foreach(RFILE IN LISTS RFILES)
    add_custom_command(TARGET copyPackage
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RFILE}" "${CMAKE_CURRENT_BINARY_DIR}/R/${RFILE}")
endforeach()

# Copy RData files
foreach(RDATAFILE IN LISTS RDATAFILES)
    add_custom_command(TARGET copyPackage
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RDATAFILE}" "${CMAKE_CURRENT_BINARY_DIR}/data/${RDATAFILE}")
endforeach()

# Copy man files
foreach(RDFILE IN LISTS RDFILES)
    add_custom_command(TARGET copyPackage
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RDFILE}" "${CMAKE_CURRENT_BINARY_DIR}/man/${RDFILE}")
endforeach()

if(WIN32)
    install(CODE "execute_process(COMMAND \"${R_COMMAND}\" CMD INSTALL --no-multiarch \$\{CMAKE_INSTALL_CONFIG_NAME\} WORKING_DIRECTORY \"${PROJECT_BINARY_DIR}\")")
else()
	install(CODE "execute_process(COMMAND \"${R_COMMAND}\" CMD INSTALL . WORKING_DIRECTORY \"${PROJECT_BINARY_DIR}\")")
endif()
