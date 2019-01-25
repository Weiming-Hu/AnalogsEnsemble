cmake_minimum_required (VERSION 2.8.12)
project (AnEnIO)
enable_language (CXX)

set (LIBRARY_NAME "AnEnIO")
set (LIBRARY_VERSION "1.0.0")

file (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Add extra source and header files for support of Array4D.
# The header file will be included in the include directories.
#
set(EXTRA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/CAnEn/include")
include_directories(${EXTRA_INCLUDE_DIR})

set(EXTRA_SOURCE_FILES "Array4D;Times;Stations;Parameters;Forecasts;Analogs;SimilarityMatrices;Observations")

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

# I'm building a static library here for CAnEnIO to avoid multiple
# copied of classes with static members. Please look at the below 
# dependency situation if both CAnEnIO and CAnEn are built as
# dynamic libraries.
#
#      Parameter (A class with a static member ID) ---->  CAnEn.so
#       |
#       |
#       V
#      CAnEnIO.so
#          
# This creates problem because the static members in two .so libraries
# clearly don't share the value even if they are the same class Parameter.
# This can also be observed if you print out the Parameter static member
# address. They are different.
# 
# To avoid this problem, I build CAnEnIO as a static library.
#          
# add_library (${LIBRARY_NAME} SHARED ${SOURCES})
add_library (${LIBRARY_NAME} STATIC ${SOURCES})

if(BOOST_TYPE STREQUAL "BUILD")
    target_link_libraries (${LIBRARY_NAME} "system")
    target_link_libraries (${LIBRARY_NAME} "filesystem")
    target_link_libraries (${LIBRARY_NAME} "program_options")
elseif(BOOST_TYPE STREQUAL "SYSTEM")
    target_link_libraries (${LIBRARY_NAME} ${Boost_LIBRARIES})
    target_link_libraries (${LIBRARY_NAME} ${Boost_LIBRARIES})
endif(BOOST_TYPE STREQUAL "BUILD")

# TODO: Install external libraries
# The external libraries are not installed to destinations. This might be changed.
# https://stackoverflow.com/questions/35934112/installing-an-externalproject-with-cmake
#
set(AnEnIO_RPATH "")

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

set_target_properties(${LIBRARY_NAME} PROPERTIES INSTALL_RPATH "${AnEnIO_RPATH}")

#if(${ENABLE_MPI})
#        include_directories(SYSTEM ${MPI_INCLUDE_PATH})
#        target_link_libraries(${LIBRARY_NAME} ${MPI_CXX_LIBRARIES})
#        if(MPI_CXX_COMPILE_FLAGS)
#            set_target_properties(${LIBRARY_NAME} PROPERTIES
#                COMPILE_FLAGS ${MPI_CXX_COMPILE_FLAGS})
#        endif(MPI_CXX_COMPILE_FLAGS)
#        if(MPI_CXX_LINK_FLAGS)
#            set_target_properties(${LIBRARY_NAME} PROPERTIES
#                LINK_FLAGS ${MPI_CXX_LINK_FLAGS})
#        endif(MPI_CXX_LINK_FLAGS)
#endif(${ENABLE_MPI}) 

# Set default make output folder
# set_target_properties(${LIBRARY_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib")
set_target_properties(${LIBRARY_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib"  # if shared
    ARCHIVE_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib") # if static

# Copy library to destination
# install(TARGETS ${LIBRARY_NAME} LIBRARY DESTINATION lib)
install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION lib)

# Copy headers to destination
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)
