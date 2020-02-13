cmake_minimum_required (VERSION 2.8.12)
project (AnEnIO)
enable_language (CXX)

set (LIBRARY_NAME "AnEnIO")

file (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(EXTRA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/CAnEn/include")
include_directories(${EXTRA_INCLUDE_DIR})

set(EXTRA_SOURCE_FILES "Times;Stations;Parameters;Forecasts;Observations")

foreach(file ${EXTRA_SOURCE_FILES})
    set(EXTRA_SOURCE_FILE "${CMAKE_SOURCE_DIR}/CAnEn/src/${file}.cpp")
    if (EXISTS ${EXTRA_SOURCE_FILE})
        list(APPEND SOURCES ${EXTRA_SOURCE_FILE})
    else(EXISTS ${EXTRA_SOURCE_FILE})
        message(FATAL_ERROR "Could not find ${EXTRA_SOURCE_FILE}")
    endif(EXISTS ${EXTRA_SOURCE_FILE})
endforeach(file ${EXTRA_SOURCE_FILES})


##################
# build and link #
##################
# add_library (${LIBRARY_NAME} SHARED ${SOURCES})
add_library (${LIBRARY_NAME} STATIC ${SOURCES})

if(BOOST_TYPE STREQUAL "BUILD")
    add_dependencies(${LIBRARY_NAME} CMAKE_BOOST)
    target_link_libraries (${LIBRARY_NAME} CMAKE_BOOST_SYSTEM)
    target_link_libraries (${LIBRARY_NAME} CMAKE_BOOST_FILESYSTEM)
    target_link_libraries (${LIBRARY_NAME} CMAKE_BOOST_PROGRAM_OPTIONS)

elseif(BOOST_TYPE STREQUAL "SYSTEM")
    target_link_libraries (${LIBRARY_NAME} ${Boost_LIBRARIES})
endif(BOOST_TYPE STREQUAL "BUILD")

if(NETCDF_CXX4_BUILT)
    include_directories(SYSTEM ${NETCDF_CXX4_INCLUDE_DIR})
    add_dependencies(${LIBRARY_NAME} NETCDF_CXX4_PROJECT)

    ExternalProject_Get_Property(NETCDF_CXX4_PROJECT install_dir)

    add_library(netcdf_cxx4_built STATIC IMPORTED)
    set_target_properties(netcdf_cxx4_built PROPERTIES
        # IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}netcdf-cxx4${CMAKE_SHARED_LIBRARY_SUFFIX}")
        IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}netcdf-cxx4${CMAKE_STATIC_LIBRARY_SUFFIX}")
    add_dependencies(netcdf_cxx4_built NETCDF_CXX4_PROJECT)

    target_link_libraries(${LIBRARY_NAME} netcdf_cxx4_built)
    set(AnEnIO_RPATH "${AnEnIO_RPATH};${install_dir}/install/lib")

endif(NETCDF_CXX4_BUILT)

include_directories(SYSTEM ${NETCDF_INCLUDES})
if (NETCDF_BUILT)
    add_dependencies(${LIBRARY_NAME} NETCDF_PROJECT)

    ExternalProject_Get_Property(NETCDF_PROJECT install_dir)

    add_library(netcdf_built SHARED IMPORTED)
    set_target_properties(netcdf_built PROPERTIES
        IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}netcdf${CMAKE_SHARED_LIBRARY_SUFFIX}")
        # IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}netcdf-cxx4${CMAKE_STATIC_LIBRARY_SUFFIX}")
    add_dependencies(netcdf_built NETCDF_PROJECT)

    target_link_libraries(${LIBRARY_NAME} netcdf_built)
    set(AnEnIO_RPATH "${AnEnIO_RPATH};${install_dir}/install/lib")

else (NETCDF_BUILT)
    target_link_libraries (${LIBRARY_NAME} ${NETCDF_LIBRARIES})
endif (NETCDF_BUILT)

if (NOT ECCODES_TYPE)
    set(ECCODES_TYPE "SYSTEM" CACHE STRING
        "The type of Eccodes library that is going to be used. Should be either SYSTEM or BUILD.")
endif (NOT ECCODES_TYPE)
message (STATUS "ECCODES_TYPE is set to ${ECCODES_TYPE}")

if (ECCODES_TYPE STREQUAL "SYSTEM")
    find_package(eccodes)
    if(eccodes_FOUND)
        message(STATUS "Eccodes include: ${ECCODES_INCLUDES}")
        message(STATUS "Eccodes lib: ${ECCODES_LIBRARIES}")
    else(eccodes_FOUND)
        message(WARNING "Eccodes library is not found. Setting the type to BUILD.")
        set(ECCODES_TYPE "BUILD")
        set(ECCODES_TYPE "BUILD" PARENT_SCOPE)
    endif(eccodes_FOUND)
endif (ECCODES_TYPE STREQUAL "SYSTEM")

if (ECCODES_TYPE STREQUAL "BUILD")
    message(STATUS "Building Eccodes library")
    include(ExternalProject)
    # This is copied from the Eccodes website
    # https://confluence.ecmwf.int/display/ECC/Releases
    #
    set(ECCODES_URL "https://confluence.ecmwf.int/download/attachments/45757960/eccodes-2.14.0-Source.tar.gz?api=v2")
    set(ECCODES_INSTALL_PREFIX "-DCMAKE_INSTALL_PREFIX=${DEPENDENCY_PATH}/install")
    set(ECCODES_BUILD_SHARED "-DBUILD_SHARED_LIBS=ON")
    set(ECCODES_DISABLE_PYTHON "-DENABLE_PYTHON=OFF")
    set(ECCODES_ROOT_PATH ${DEPENDENCY_PATH})
    set(ECCODES_CMAKE_ARGS ${ECCODES_DISABLE_PYTHON} ${ECCODES_INSTALL_PREFIX} ${ECCODES_BUILD_SHARED})

    if (CMAKE_PREFIX_PATH)
        # This if statement and the code have several purposes:
        # - If CMAKE_PREFIX_PATH is set, pass it to external project.
        # - Deal with CMAKE_PREFIX_PATH with multiple folders by changing the separator.
        #
        string(REPLACE ";" "|" ECCODES_ALT_CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}")
        set(ECCODES_PREFIX_PATH "-DCMAKE_PREFIX_PATH=${ECCODES_ALT_CMAKE_PREFIX_PATH}")
        set(ECCODES_CMAKE_ARGS ${ECCODES_CMAKE_ARGS} ${ECCODES_PREFIX_PATH})
    endif(CMAKE_PREFIX_PATH)

    if (OPENJPEG_INCLUDE_DIR)
        set(ECCODES_OPENJPEG "-DJASPER_INCLUDE_DIR=${OPENJPEG_INCLUDE_DIR}")
        set(ECCODES_CMAKE_ARGS ${ECCODES_CMAKE_ARGS} ${ECCODES_OPENJPEG})
    endif(OPENJPEG_INCLUDE_DIR)
    
    if (JASPER_INCLUDE_DIR)
        set(ECCODES_JASPER "-DJASPER_INCLUDE_DIR=${JASPER_INCLUDE_DIR}")
        set(ECCODES_CMAKE_ARGS ${ECCODES_CMAKE_ARGS} ${ECCODES_JASPER})
    endif(JASPER_INCLUDE_DIR)

    if(VERBOSE)
        set(VERBOSE_DOWNLOAD 0)
        set(VERBOSE_CONFIGURE 0)
        set(VERBOSE_BUILD 0) 
        set(VERBOSE_INSTALL 0) 
        set(VERBOSE_TEST 0)
    else(VERBOSE)
        set(VERBOSE_DOWNLOAD 1)
        set(VERBOSE_CONFIGURE 1)
        set(VERBOSE_BUILD 1) 
        set(VERBOSE_INSTALL 1) 
        set(VERBOSE_TEST 1)
    endif(VERBOSE)

    ExternalProject_add(ECCODES_PROJECT
        PREFIX ${ECCODES_ROOT_PATH}
        URL ${ECCODES_URL}
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        LIST_SEPARATOR | # Use the alternate list separator
        CMAKE_ARGS ${ECCODES_CMAKE_ARGS}
        BUILD_COMMAND ${MAKE}
        LOG_TEST ${VERBOSE_TEST}
        LOG_DOWNLOAD ${VERBOSE_DOWNLOAD}
        LOG_CONFIGURE ${VERBOSE_CONFIGURE}
        LOG_BUILD ${VERBOSE_BUILD}
        LOG_INSTALL ${VERBOSE_INSTALL})

    set (ECCODES_INCLUDE_DIRS "${DEPENDENCY_PATH}/install/include" CACHE STRING "ECCODES include dirs")
endif (ECCODES_TYPE STREQUAL "BUILD")

include_directories(${ECCODES_INCLUDE_DIRS})
if (ECCODES_TYPE STREQUAL "SYSTEM")
    target_link_libraries(${LIBRARY_NAME} ${ECCODES_LIBRARIES})
else (ECCODES_TYPE STREQUAL "SYSTEM")
    ExternalProject_Get_Property(ECCODES_PROJECT install_dir)
    add_library(eccodes_built SHARED IMPORTED)
    set(ECCODES_IMPORT_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}eccodes${CMAKE_SHARED_LIBRARY_SUFFIX}")
    message(STATUS "Import location for Eccodes: ${ECCODES_IMPORT_LOCATION}")

    set_target_properties(eccodes_built PROPERTIES
        IMPORTED_LOCATION ${ECCODES_IMPORT_LOCATION})
    add_dependencies(eccodes_built ECCODES_PROJECT)
    target_link_libraries(${LIBRARY_NAME} eccodes_built)

endif (ECCODES_TYPE STREQUAL "SYSTEM")

set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "${AnEnIO_RPATH}")

get_target_property(${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES ${LIBRARY_NAME} INTERFACE_LINK_LIBRARIES)
if ("${${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES}" STREQUAL "${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES-NOTFOUND")
    set(${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES ${NETCDF_LINKING})
else ("${${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES}" STREQUAL "${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES-NOTFOUND")
    set(${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES "${${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES};${NETCDF_LINKING}")
endif ("${${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES}" STREQUAL "${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES-NOTFOUND")

get_target_property(${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS ${LIBRARY_NAME} INTERFACE_COMPILE_OPTIONS)
if ("${${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS}" STREQUAL "${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS-NOTFOUND")
    set(${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS ${NETCDF_CFLAGS})
else ("${${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS}" STREQUAL "${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS-NOTFOUND")
    set(${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS "${${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS};${NETCDF_CFLAGS}")
endif ("${${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS}" STREQUAL "${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS-NOTFOUND")

message(STATUS "The compile options for " ${LIBRARY_NAME} ": ${${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS}")
message(STATUS "The linking flags for " ${LIBRARY_NAME} ": ${${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES}")

# Set default make output folder
# set_target_properties(${LIBRARY_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib")
set_target_properties(${LIBRARY_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib"  # if shared
    ARCHIVE_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib"  # if static
    INTERFACE_LINK_LIBRARIES "${${LIBRARY_NAME}_INTERFACE_LINK_LIBRARIES}"
    INTERFACE_COMPILE_OPTIONS "${${LIBRARY_NAME}_INTERFACE_COMPILE_OPTIONS}")

# Copy library to destination
# install(TARGETS ${LIBRARY_NAME} LIBRARY DESTINATION lib)
install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION lib)

# Copy headers to destination
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)
