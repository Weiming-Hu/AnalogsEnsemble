###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This is the super project file that controls how to build the following sub
# projects:
#
# - AnEn
# - AnEnIO
# - RAnEn
#

cmake_minimum_required(VERSION 3.12)
project(PAnEn LANGUAGES C CXX)

# Whether to build RAnEn
option(INSTALL_RAnEn "Install RAnEn package" OFF)

# Whether to build boost
option(BUILD_BOOST "Build Boost" OFF)

# Set variables
set(GRAND_VERSION "4.0.1")
set(GRAND_VERSION_NICKNAME "Pandora")
set(WE_ARE "GEOlab @ Penn State")
set(SUBPROJECT_TARGETS "AnEn;AnEnIO")
set(EXTRA_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${EXTRA_MODULE_PATH})

# Add defitions to project space so that they affect all sub projects
add_definitions(-D_APPVERSION="v ${GRAND_VERSION}")
add_definitions(-D_COPYRIGHT_MSG="${WE_ARE}")
add_definitions(-DBOOST_NO_AUTO_PTR)

include("${EXTRA_MODULE_PATH}/macros.cmake")

message(STATUS "You are building PAnEn version " ${GRAND_VERSION} " "
    ${GRAND_VERSION_NICKNAME} " from " ${WE_ARE})

# Go to RAnEn build procedures if it is requested
if(INSTALL_RAnEn)
    message(STATUS "Configuring RAnEn package")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/RAnalogs")
    return()
endif(INSTALL_RAnEn)


# Build Boost if requested
# 
# Some libraries like date_time, filesystem, and program_options, needs
# compilation. This option is helpful when you are using a different
# compiler to build PAnEn than the one you used to build Boost. For example,
# on Mac OS, you probably want to use GNU compiler for OpenMP support, but
# usually, Boost (if you used brew install boost) was built with Clang which
# will cause problems. Then this option is needed.
#
# Since users should not be concerning about this, I default this to ON so
# Boost will be built automatically. Only the submodules needed by PAnEn will
# be built.
#
if(BUILD_BOOST)
    list(APPEND SUBPROJECT_TARGETS "Boost")
    check_boost_download()
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/boost-cmake)
endif(BUILD_BOOST)


# Add libraries as subprojects
add_subdirectory(CAnEn)
add_subdirectory(CAnEnIO)

# Add applications as subprojects
add_subdirectory(apps/anen)

find_package(CppUnit)
if(CPPUNIT_FOUND)
    message(STATUS "CppUnit found. Building tests")
    enable_testing()
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
else(CPPUNIT_FOUND)
    message(STATUS "No tests built. Install CppUnit to automatically enable tests")
endif(CPPUNIT_FOUND)

# Use doxygen to generate documents for C++
find_package (Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_QUIET "NO")
    set(DOXYGEN_GENERATE_XML "NO")
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN YES)
    set(DOXYGEN_GENERATE_LATEX "NO")
    set(DOXYGEN_PROJECT_NAME "Parallel Analog Ensemble")
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(DOXYGEN_MAINPAGE_MDFILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(DOXYGEN_EXCLUDE_PATTERNS "*build* *nbproject* *RAnalogs* *cmake*")

    if (${DOXYGEN_DOT_FOUND})
        set (DOXYGEN_HAVE_DOT YES)
        message (STATUS "Graphviz is found.")
    else (${DOXYGEN_DOT_FOUND}) 
        set (DOXYGEN_HAVE_DOT "NO")
        message (STATUS "Graphviz is NOT found. No DOT support.")
    endif (${DOXYGEN_DOT_FOUND})

    configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/doxygen-config.in" "${CMAKE_CURRENT_BINARY_DIR}/doxygen-config")

    message (STATUS "Type 'make document' to generate C++ documentation.")

    add_custom_target(document
        COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/doxygen-config"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generate help page using doxygen")
endif (DOXYGEN_FOUND)


message(STATUS "\n"
    "**********************************************************************\n\n"
    "If the build system generation is complete,\n"
    "you can type 'make' or 'make -j <number of cores>' to build projects.\n\n"
    "If you have set -DCMAKE_INSTALL_PREFIX=[path],\n"
    "you can also type 'make install' to build and install projects\n\n"
    "**********************************************************************")

