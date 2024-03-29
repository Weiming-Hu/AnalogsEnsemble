###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This file builds RAnEn with CMake

cmake_minimum_required(VERSION 3.1)
project(RAnEn LANGUAGES CXX C)

# Find packages
find_package(OpenMP)
find_package(R REQUIRED)
message(STATUS "R is found at ${R_LIBRARY_BASE}")

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
        message(FATAL_ERROR
			"Rscript is not found in ${R_PATH}. Please use R CMD INSTALL utility.")
    else (RSCRIPT_COMMAND STREQUAL "RSCRIPT_COMMAND-NOTFOUND")
        message(STATUS "Rscript: ${RSCRIPT_COMMAND}.")
    endif (RSCRIPT_COMMAND STREQUAL "RSCRIPT_COMMAND-NOTFOUND")
endif (RSCRIPT_COMMAND)

# Use Rscript to find Cxx flags for Rcpp
execute_process(COMMAND ${RSCRIPT_COMMAND} -e "Rcpp:::CxxFlags()"
	OUTPUT_VARIABLE RCPP_CXXFLAGS)
message(STATUS "Rcpp CxxFlags: ${RCPP_CXXFLAGS}.")

# Find Rcpp path
# Because CMake does not support non-greedy mode
# I have to use index approach to get rid of the prefix option
#
string (FIND ${RCPP_CXXFLAGS} "\/" PATH_START_INDEX)
string (SUBSTRING ${RCPP_CXXFLAGS} ${PATH_START_INDEX} -1 RCPP_DIR)
string(REGEX REPLACE "^(.+)\/include.*" \\1 RCPP_DIR ${RCPP_DIR})
string(REGEX REPLACE "^(.+)\/Rcpp.*" \\1 R_LIBS_DIR ${RCPP_DIR})

message(STATUS RCPP_DIR: ${RCPP_DIR})
message(STATUS R_LIBS_DIR: ${R_LIBS_DIR})

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

# Import Rcpp library
if (WIN32)
    message(FATAL_ERROR "The CMake installation approach is not supported. Please use R CMD INSTALL.")
endif (WIN32)

add_library(Rcpp SHARED IMPORTED)
set_target_properties(Rcpp PROPERTIES IMPORTED_LOCATION "${RCPP_DIR}/libs/Rcpp.so")

# Link the directory for this imported library manually. If I don't do so, Rcpp.so is always
# linked relatively on Mac OS which causes problems. By manually linking the directory, all
# targets created after this will be linked to the full directory and I don't need to link
# Rcpp again.
#
link_directories("${RCPP_DIR}/libs/")

# Add the shared libarry target
file (GLOB SourceFiles "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(RAnEn SHARED ${SourceFiles})
set_property(TARGET RAnEn PROPERTY PREFIX "")
set_property(TARGET RAnEn PROPERTY LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src")
set_property(TARGET RAnEn PROPERTY SUFFIX ".so")

# Include and link libraries
set_property(TARGET RAnEn APPEND PROPERTY COMPILE_FLAGS ${RCPP_CXXFLAGS})
include_directories (SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/src" ${R_INCLUDE_DIR})
target_link_libraries(RAnEn PUBLIC ${R_LIBRARY_BASE})

if(OpenMP_CXX_FOUND)
    target_link_libraries(RAnEn PUBLIC OpenMP::OpenMP_CXX)
endif(OpenMP_CXX_FOUND)

if (R_BH_INCLUDE_DIR)
    include_directories (SYSTEM ${R_BH_INCLUDE_DIR})
endif (R_BH_INCLUDE_DIR)

# Enable C++14: For later R versions, C++14 is preferred than C++11 to avoid compilation errors.
if (NOT COMPILER_SUPPORTS_CXX14)
    set_property(TARGET RAnEn PROPERTY CXX_STANDARD 14)
    set_property(TARGET RAnEn PROPERTY CXX_STANDARD_REQUIRED ON)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
        target_compile_options(RAnEn PUBLIC "-std=c++14")
	endif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
endif (NOT COMPILER_SUPPORTS_CXX14)

# Add target to copy files
add_custom_target(copyPackage ALL)	

# Set files to copy
file (GLOB_RECURSE COPYFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")

# Copy files
foreach(COPYFILE IN LISTS COPYFILES)
	add_custom_command(TARGET copyPackage
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/${COPYFILE}"
		"${CMAKE_CURRENT_BINARY_DIR}/${COPYFILE}")
endforeach(COPYFILE IN LISTS COPYFILES)

# Copy empty Makefiles to suppress any compiling of the source code when
# using make install to install R package because the RAnEn.so should already
# be compiled.
#
add_custom_command(TARGET copyPackage
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${EXTRA_MODULE_PATH}/Makefile.win"
    "${CMAKE_CURRENT_BINARY_DIR}/src/Makefile.win")
add_custom_command(TARGET copyPackage
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${EXTRA_MODULE_PATH}/Makefile"
    "${CMAKE_CURRENT_BINARY_DIR}/src/Makefile")

install(CODE "execute_process(COMMAND \"${R_COMMAND}\"
CMD INSTALL . WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")")

file(GLOB FILES_TO_DELETE_ROXYGEN "${CMAKE_CURRENT_SOURCE_DIR}/src/*.o")
list(APPEND FILES_TO_DELETE_ROXYGEN "${CMAKE_CURRENT_SOURCE_DIR}/src/RAnEn.so")

add_custom_target(
    clean-roxygen
    COMMAND ${CMAKE_COMMAND} -E remove ${FILES_TO_DELETE_ROXYGEN}
    COMMENT "clean *.o *.so files possibly created by Roxygen.")

if(TARGET clean-all)
    add_dependencies(clean-all clean-roxygen)
else(TARGET clean-all)
    add_custom_target(clean-all DEPENDS clean-roxygen)
endif(TARGET clean-all)
