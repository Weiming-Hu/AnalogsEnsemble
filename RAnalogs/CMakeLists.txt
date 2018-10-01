cmake_minimum_required (VERSION 3.1)
project (RAnalogs LANGUAGES NONE)

set(LIBRARY_NAME "RAnEn")
set(LIBRARY_VERSION "3.0.5")
message(STATUS "RAnEn version ${LIBRARY_VERSION}")

set(RAnEn_PACKAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_NAME}")
set(RAnEn_SOURCE_DIR "${RAnEn_PACKAGE_DIR}/src")

set(RAnEn_EXTRA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/CAnEn/include")
set(RAnEn_EXTRA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/CAnEn/src")
set(RAnEn_RELEASES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/releases")


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

# Configure DESCRIPTION
string(TIMESTAMP LIBRARY_CONFIGURE_TIMESTAMP "%Y-%m-%d_%H:%M")
configure_file("${RAnEn_PACKAGE_DIR}/DESCRIPTION.in" "${RAnEn_PACKAGE_DIR}/DESCRIPTION")

# copy files
set(REQUIRED_FILE_NAME "Analogs;AnEn;Array4D;Forecasts;Observations;Parameters;SimilarityMatrices;StandardDeviation;Stations;Times")

foreach(file_name ${REQUIRED_FILE_NAME})
    file(COPY "${RAnEn_EXTRA_INCLUDE_DIR}/${file_name}.h"
        DESTINATION ${RAnEn_SOURCE_DIR})
    file(COPY "${RAnEn_EXTRA_SOURCE_DIR}/${file_name}.cpp"
        DESTINATION ${RAnEn_SOURCE_DIR})
endforeach(file_name ${REQUIRED_FILE_NAME})

file(COPY "${RAnEn_EXTRA_INCLUDE_DIR}/colorTexts.h" DESTINATION ${RAnEn_SOURCE_DIR})

add_subdirectory(${RAnEn_PACKAGE_DIR})


##########################
# customize clean target #
##########################
#
set(FILES_TO_DELETE "")
foreach(file ${REQUIRED_FILE_NAME})
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file}.h")
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file}.cpp")
    set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/${file}.o")
endforeach(file ${REQUIRED_FILE_NAME})
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/colorTexts.h")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/RAnEn.so")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/Makefile")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/RcppExports.o")
set(FILES_TO_DELETE "${FILES_TO_DELETE};${RAnEn_SOURCE_DIR}/Rinterface.o")

message(STATUS "Add clean-rsrc target for cleaning the copied files")
add_custom_target(
    clean-rsrc 
    COMMAND ${CMAKE_COMMAND} -E remove ${FILES_TO_DELETE}
    COMMENT "clean the copied files in RAnEn/src. You will need to cmake again if you want to make.")

# Add target to build release
add_custom_target(build-release)	

add_custom_command(TARGET build-release
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/README.md"
    "${RAnEn_PACKAGE_DIR}/README.md")
add_custom_command(TARGET build-release
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${RAnEn_PACKAGE_DIR}/cmake/configure"
    "${RAnEn_PACKAGE_DIR}/configure")
add_custom_command(TARGET build-release
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${RAnEn_PACKAGE_DIR}/cmake/Makevars.in"
    "${RAnEn_SOURCE_DIR}/Makevars.in")
add_custom_command(TARGET build-release
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${RAnEn_PACKAGE_DIR}/cmake/Makevars.win.in"
    "${RAnEn_SOURCE_DIR}/Makevars.win.in")

add_custom_command(
    TARGET build-release
    COMMAND ${R_COMMAND} CMD build ${RAnEn_PACKAGE_DIR}
    COMMENT "target for building RAnEn release tarbal"
    WORKING_DIRECTORY ${RAnEn_RELEASES_DIR})

add_custom_command(TARGET build-release
    COMMAND ${CMAKE_COMMAND} -E remove "${RAnEn_PACKAGE_DIR}/configure")
add_custom_command(TARGET build-release
    COMMAND ${CMAKE_COMMAND} -E remove "${RAnEn_SOURCE_DIR}/Makevars")
add_custom_command(TARGET build-release
    COMMAND ${CMAKE_COMMAND} -E remove "${RAnEn_SOURCE_DIR}/Makevars.win")

# Remove old release tarball file if exists
set(OLD_RAnEn_TARFILE "${LIBRARY_NAME}_${LIBRARY_VERSION}.tar.gz")
add_custom_target(
	remove-old-release
	COMMAND ${CMAKE_COMMAND} -E remove ${OLD_RAnEn_TARFILE}
	COMMENT "target to remove the old release ${OLD_RAnEn_TARFILE}"
	WORKING_DIRECTORY ${RAnEn_RELEASES_DIR})
add_dependencies(build-release remove-old-release)


