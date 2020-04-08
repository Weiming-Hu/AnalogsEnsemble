###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This script sets up the build environment for the RAnEn package. It copies the 
# source files from CAnEn and CAnEnIO directories to RAnEn directory and then defines
# some targets to build release version and clean copied files. It also configures
# some *.in files with the correct setting from the superproject.
#
# This script does not actually carry out the installation process.
#

cmake_minimum_required (VERSION 3.1)

set(LIBRARY_NAME "RAnEn")
set(LIBRARY_VERSION "${GRAND_VERSION}")
set(LIBRARY_VERSION_NICKNAME "${GRAND_VERSION_NICKNAME}")
set(RAnEn_PACKAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_NAME}")
set(RAnEn_SOURCE_DIR "${RAnEn_PACKAGE_DIR}/src")
set(RAnEn_EXTRA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/CAnEn/include")
set(RAnEn_EXTRA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/CAnEn/src")
set(RAnEn_RELEASES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/releases")

message(STATUS "Preparing RAnEn version ${LIBRARY_VERSION}")

# Disable the Boost libraries that need compilation. For more information about
# this definition, please see the source file for Functions::toSeconds
#
add_definitions(-D_DISABLE_NON_HEADER_BOOST)

##########################
# complete src directory #
##########################
#
# RAnEn needs the source files from CAnEn and CAnEnIO, we will copy them from
# the original directory every time we build our R package.
#
# By doing this, we make sure
#
# 1) we don't maintain the same code at two different places;
# 2) we always build the R package using the latest C++ code;
#
# make sure all the required source and header files exist
foreach(CHECK_PATH ${RAnEn_SOURCE_DIR} ${RAnEn_EXTRA_SOURCE_DIR} ${RAnEn_EXTRA_INCLUDE_DIR})
    if(NOT EXISTS ${CHECK_PATH})
        message(FATAL_ERROR "${CHECK_PATH} not found")
    endif(NOT EXISTS ${CHECK_PATH})
endforeach(CHECK_PATH ${RAnEn_SOURCE_DIR} ${RAnEn_EXTRA_SOURCE_DIR} ${RAnEn_EXTRA_INCLUDE_DIR})

# Configure DESCRIPTION
string(TIMESTAMP LIBRARY_CONFIGURE_TIMESTAMP "%Y-%m-%d_%H:%M")
configure_file("${RAnEn_PACKAGE_DIR}/DESCRIPTION.in" "${RAnEn_PACKAGE_DIR}/DESCRIPTION")

# These are the files with different types that will be copied
set(REQUIRED_SOURCE_FILES "AnEn;Array4DPointer;BasicData;Calculator;Config;Forecasts;ForecastsPointer;Profiler")
list(APPEND REQUIRED_SOURCE_FILES "Observations;ObservationsPointer;Parameters;Stations;Times")
set(REQUIRED_TEMPLATE_FILES "AnEnIS;AnEnSSE;Functions")
set(REQUIRED_HEADER_ONLY_FILES "BmDim;Array4D")

foreach(file_name ${REQUIRED_SOURCE_FILES})
    file(COPY "${RAnEn_EXTRA_INCLUDE_DIR}/${file_name}.h" DESTINATION ${RAnEn_SOURCE_DIR})
    file(COPY "${RAnEn_EXTRA_SOURCE_DIR}/${file_name}.cpp" DESTINATION ${RAnEn_SOURCE_DIR})
endforeach(file_name ${REQUIRED_SOURCE_FILES})

foreach(file_name ${REQUIRED_TEMPLATE_FILES})
    file(COPY "${RAnEn_EXTRA_INCLUDE_DIR}/${file_name}.h" DESTINATION ${RAnEn_SOURCE_DIR})
    file(COPY "${RAnEn_EXTRA_INCLUDE_DIR}/${file_name}.tpp" DESTINATION ${RAnEn_SOURCE_DIR})
    file(COPY "${RAnEn_EXTRA_SOURCE_DIR}/${file_name}.cpp" DESTINATION ${RAnEn_SOURCE_DIR})
endforeach(file_name ${REQUIRED_TEMPLATE_FILES})

foreach(file_name ${REQUIRED_HEADER_ONLY_FILES})
    file(COPY "${RAnEn_EXTRA_INCLUDE_DIR}/${file_name}.h" DESTINATION ${RAnEn_SOURCE_DIR})
endforeach(file_name ${REQUIRED_HEADER_ONLY_FILES})

file(COPY "${CMAKE_SOURCE_DIR}/NEWS.md" DESTINATION ${RAnEn_PACKAGE_DIR})
file(COPY "${EXTRA_MODULE_PATH}/configure" DESTINATION ${RAnEn_PACKAGE_DIR})
file(COPY "${EXTRA_MODULE_PATH}/Makevars.in" DESTINATION ${RAnEn_SOURCE_DIR})
file(COPY "${EXTRA_MODULE_PATH}/Makevars.win" DESTINATION ${RAnEn_SOURCE_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/README.md" DESTINATION ${RAnEn_PACKAGE_DIR})

add_subdirectory(${RAnEn_PACKAGE_DIR})


##########################
# customize clean target #
##########################
#
file(GLOB FILES_TO_DELETE "${RAnEn_SOURCE_DIR}/*.o")
foreach(file_name ${REQUIRED_SOURCE_FILES})
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file_name}.cpp")
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file_name}.h")
endforeach(file_name ${REQUIRED_SOURCE_FILES})

foreach(file_name ${REQUIRED_TEMPLATE_FILES})
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file_name}.tpp")
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file_name}.cpp")
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file_name}.h")
endforeach(file_name ${REQUIRED_TEMPLATE_FILES})

foreach(file_name ${REQUIRED_HEADER_ONLY_FILES})
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file_name}.h")
endforeach(file_name ${REQUIRED_HEADER_ONLY_FILES})

set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/RAnEn.so")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_PACKAGE_DIR}/config.log")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_PACKAGE_DIR}/DESCRIPTION")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/Makefile")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_PACKAGE_DIR}/configure")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/README.md")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/NEWS.md")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/Makevars.in")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/Makevars.win")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/Makevars")

add_custom_target(
    clean-rsrc 
    COMMAND ${CMAKE_COMMAND} -E remove ${FILES_TO_DELETE}
    COMMAND ${CMAKE_COMMAND} -E remove ${RAnEn_SOURCE_DIR}/*.o
    COMMENT "clean the copied files in RAnEn/src. You will need to cmake again if you want to make.")

message(STATUS "Use `make clean-all` to clean all intermediate files during RAnEn compilation and installation")
if(TARGET clean-all)
    add_dependencies(clean-all clean-rsrc)
else(TARGET clean-all)
    add_custom_target(clean-all DEPENDS clean-rsrc)
endif(TARGET clean-all)

# Add target to build release
message(STATUS "Use `make build-release` to make a release with the current version")
add_custom_target(build-release)	

add_custom_command(
    TARGET build-release
    COMMAND ${R_COMMAND} CMD build ${RAnEn_PACKAGE_DIR}
    COMMENT "target for building RAnEn release tarbal"
    WORKING_DIRECTORY ${RAnEn_RELEASES_DIR})

add_custom_command(TARGET build-release
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/releases/${LIBRARY_NAME}_${LIBRARY_VERSION}.tar.gz"
    "${CMAKE_CURRENT_SOURCE_DIR}/releases/${LIBRARY_NAME}_latest.tar.gz")

# Remove old release tarball file if exists
set(OLD_RAnEn_TARFILE "${LIBRARY_NAME}_${LIBRARY_VERSION}.tar.gz")
add_custom_target(
	remove-old-release
	COMMAND ${CMAKE_COMMAND} -E remove ${OLD_RAnEn_TARFILE}
	COMMENT "target to remove the old release ${OLD_RAnEn_TARFILE}"
	WORKING_DIRECTORY ${RAnEn_RELEASES_DIR})
add_dependencies(build-release remove-old-release)


