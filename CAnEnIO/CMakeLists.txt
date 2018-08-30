cmake_minimum_required (VERSION 2.8.12)
project (AnEnIO)
enable_language (CXX)

set (LIBRARY_NAME "AnEnIO")
set (LIBRARY_VERSION "2.0.0")

file (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")


# Add extra source and header files for support of Array4D.
# The header file will be included in the include directories.
#
set(EXTRA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/CAnEn/include")
include_directories(${EXTRA_INCLUDE_DIR})

set(EXTRA_SOURCE_FILES "Times;Stations;Parameters;Forecasts;Analogs;SimilarityMatrices;Observations")

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
add_library (${LIBRARY_NAME} SHARED ${SOURCES})

include_directories(${NETCDF_CXX4_INCLUDE_DIR})
add_dependencies(${LIBRARY_NAME} NETCDF_CXX4_PROJECT)

# TODO: install external libraries
# https://stackoverflow.com/questions/35934112/installing-an-externalproject-with-cmake
set(AnEnIO_RPATH "")

#if(HDF5_BUILT)
#    include(ExternalProject)
#    ExternalProject_Get_Property(HDF5_PROJECT install_dir)
#
#    add_library(hdf5_built SHARED IMPORTED)
#    set_target_properties(hdf5_built
#        PROPERTIES IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}hdf5${CMAKE_SHARED_LIBRARY_SUFFIX}")
#    add_dependencies(hdf5_built HDF5_PROJECT)
#
#    add_library(hdf5_hl_built SHARED IMPORTED)
#    set_target_properties(hdf5_hl_built
#        PROPERTIES IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}hdf5_hl${CMAKE_SHARED_LIBRARY_SUFFIX}")
#    add_dependencies(hdf5_hl_built HDF5_PROJECT)
#
#    target_link_libraries(${LIBRARY_NAME} hdf5_hl_built hdf5_built)
#    set(AnEnIO_RPATH "${AnEnIO_RPATH};${install_dir}/install/lib")
#
#else(HDF5_BUILT)
#    target_link_libraries(${LIBRARY_NAME} ${HDF5_C_LIBRARY} ${HDF5_HL_LIBRARY})
#endif(HDF5_BUILT)
#
#if(NETCDF_BUILT)
#    ExternalProject_Get_Property(NETCDF_PROJECT install_dir)
#
#    add_library(netcdf_built SHARED IMPORTED)
#    set_target_properties(netcdf_built
#        PROPERTIES IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}netcdf${CMAKE_SHARED_LIBRARY_SUFFIX}")
#    add_dependencies(netcdf_built NETCDF_PROJECT)
#
#    target_link_libraries(${LIBRARY_NAME} netcdf_built)
#    set(AnEnIO_RPATH "${AnEnIO_RPATH};${install_dir}/install/lib")
#
#else(NETCDF_BUILT)
#    target_link_libraries(${LIBRARY_NAME} ${NETCDF_LIBRARIES})
#endif(NETCDF_BUILT)

if(NETCDF_CXX4_BUILT)
    ExternalProject_Get_Property(NETCDF_CXX4_PROJECT install_dir)

    add_library(netcdf_cxx4_built SHARED IMPORTED)
    set_target_properties(netcdf_cxx4_built
        PROPERTIES IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}netcdf-cxx4${CMAKE_SHARED_LIBRARY_SUFFIX}")
    add_dependencies(netcdf_cxx4_built NETCDF_CXX4_PROJECT)

    target_link_libraries(${LIBRARY_NAME} netcdf_cxx4_built)
    set(AnEnIO_RPATH "${AnEnIO_RPATH};${install_dir}/install/lib")

else(NETCDF_CXX4_BUILT)
    message(FATAL "netcdf-cxx4 should always be built.")
endif(NETCDF_CXX4_BUILT)

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
set_target_properties(${LIBRARY_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib")

# Copy library to destination
install(TARGETS ${LIBRARY_NAME} LIBRARY DESTINATION lib)

# Copy headers to destination
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)
