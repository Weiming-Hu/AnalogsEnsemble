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

cmake_minimum_required(VERSION 3.10)
project(PAnEn LANGUAGES C CXX)

option(INSTALL_RAnEn "Install RAnEn package" OFF)
option(BUILD_BOOST "Build Boost" OFF)
option(ENABLE_OPENMP "Use OpenMP" ON)
option(ENABLE_MPI "Use MPI" OFF)
option(ENABLE_AI "Use AI for analog search" OFF)

# Set variables
set(GRAND_VERSION "4.4.2")
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

if(ENABLE_AI)
    find_package(Torch)

    if(Torch_FOUND)
        message(STATUS "\n"
            "*************************************\n\n"
            "You have enabled the AI component!\n"
            "Enjoy the power of AI!\n\n"
            "*************************************")
    else(Torch_FOUND)
        message(FATAL_ERROR  "\n"
            "I couldn't find LibTorch. There are probably some more setting to do.\n"
            "You need to:\n"
            "-- download LibTorch for C++/Java with cxx11 ABI from https://pytorch.org/\n"
            "-- unzip it\n"
            "-- include the directory of the folder in CMAKE_PREFIX_PATH.\n"
            "-- include the directory of the folder in CMAKE_INSTALL_RPATH if you plan to run `make install` after `make`.\n"
            "-- specify the python executable if not default in PYTHON_EXECUTABLE.\n"
            "-- build shared libraries with -DBUILD_SHARED_LIBS=ON.\n"
            "If you still have problems, please open a ticket or check https://github.com/Weiming-Hu/AnalogsEnsemble/issues/110.\n"
            "If you have multiple paths in CMAKE_PREFIX_PATH or CMAKE_INSTALL_RPATH, use double quotes and ; as the separator.\n"
            "An example for cmake would be: cmake -DPYTHON_EXECUTABLE=[python executable] -DBUILD_SHARED_LIBS=ON -DENABLE_AI=ON"
            " -DCMAKE_INSTALL_RPATH=[lib folder] -DCMAKE_PREFIX_PATH=[libtorch root] -DCMAKE_INSTALL_PREFIX=[install path] ..")
    endif(Torch_FOUND)

    add_definitions(-D_ENABLE_AI)
endif(ENABLE_AI)

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
    # This is to include Boost as a subproject
    list(APPEND SUBPROJECT_TARGETS "Boost")

    # Boost-cmake currently does not support install rules
    set(CMAKE_SKIP_INSTALL_RULES TRUE)
    message(STATUS "Installation rules will be disabled because boost-cmake does not support install rules")
    message(STATUS "More information please refer to https://github.com/Orphis/boost-cmake/pull/45")

    # Check source downloading
    check_boost_cmake_download()

    # Include the directory as a subproject
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/boost-cmake)

endif(BUILD_BOOST)

# I'm not using Boost directly in this direct file, rather I'm just checking the
# version of Boost library to see whether I should include a certain compiler
# definition.
#
find_package(Boost 1.58.0 REQUIRED)

# For this specific version of Boost, I need to change the header files to include.
# Therefore, I have a compiler definition to take care of this.
#
if(Boost_MAJOR_VERSION STREQUAL "1")
    if (Boost_MINOR_VERSION STREQUAL "58")
        message(STATUS "Add compiler definition for Boost version 1.58.x (_BOOST_VERSION_1_58_)")
        add_definitions(-D_BOOST_VERSION_1_58_)
    endif(Boost_MINOR_VERSION STREQUAL "58")
endif(Boost_MAJOR_VERSION STREQUAL "1")

# Add libraries as subprojects
add_subdirectory(CAnEn)
add_subdirectory(CAnEnIO)
if(ENABLE_AI)
    add_subdirectory(CGrid)
    list(APPEND SUBPROJECT_TARGETS "Grid")
endif(ENABLE_AI)

if(ENABLE_MPI)
    if(ENABLE_OPENMP)
        message(STATUS "
***************************************************************

You have both ENABLE_MPI and ENABLE_OPENMP ON.

Please be aware that this makes the AnEn programs
hybrid of MPI (multi-node) and OpenMP (multi-thread).

Please check with your individual HPC platform to ensure
the correct configuration to launch a hybrid MPI+OpenMP program.

Programs MIGHT HANG when you are not launching the program properly
or if you are overtasking the machine.

Please refer to the README for suggestions and more information.
https://weiming-hu.github.io/AnalogsEnsemble/doc#mpi-openmp

***************************************************************
        ")
    endif(ENABLE_OPENMP)

    list(APPEND SUBPROJECT_TARGETS "AnEnIOMPI" "AnEnMPI")
    add_subdirectory(CAnEnIOMPI)
    add_subdirectory(CAnEnMPI)

endif(ENABLE_MPI)

# Add applications as subprojects
add_subdirectory(apps/anen_grib)
add_subdirectory(apps/anen_netcdf)
add_subdirectory(apps/grib_convert)

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

