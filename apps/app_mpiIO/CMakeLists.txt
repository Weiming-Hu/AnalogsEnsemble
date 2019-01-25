cmake_minimum_required(VERSION 2.8)
project(mpiIO)
enable_language(C CXX)

add_definitions(-D_APPVERSION="v 0.0.0")

message(STATUS "Build MPI functions and utilities for NetCDF I/O")
set(SOURCE_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/mpiIO.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_MAIN})

if(NETCDF_CXX4_BUILT)
    include_directories(SYSTEM ${NETCDF_CXX4_INCLUDE_DIR})
    add_dependencies(${PROJECT_NAME} NETCDF_CXX4_PROJECT)

    ExternalProject_Get_Property(NETCDF_CXX4_PROJECT install_dir)

    add_library(netcdf_cxx4_built STATIC IMPORTED)
    set_target_properties(netcdf_cxx4_built PROPERTIES
        # IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}netcdf-cxx4${CMAKE_SHARED_LIBRARY_SUFFIX}")
        IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}netcdf-cxx4${CMAKE_STATIC_LIBRARY_SUFFIX}")
    add_dependencies(netcdf_cxx4_built NETCDF_CXX4_PROJECT)

    target_link_libraries(${PROJECT_NAME} netcdf_cxx4_built)
    set(AnEnIO_RPATH "${AnEnIO_RPATH};${install_dir}/install/lib")
endif(NETCDF_CXX4_BUILT)

include_directories(SYSTEM ${NETCDF_INCLUDES})

if (NETCDF_BUILT)
    add_dependencies(${PROJECT_NAME} NETCDF_PROJECT)

    ExternalProject_Get_Property(NETCDF_PROJECT install_dir)

    add_library(netcdf_built SHARED IMPORTED)
    set_target_properties(netcdf_built PROPERTIES
        IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}netcdf${CMAKE_SHARED_LIBRARY_SUFFIX}")
        # IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}netcdf-cxx4${CMAKE_STATIC_LIBRARY_SUFFIX}")
    add_dependencies(netcdf_built NETCDF_PROJECT)

    target_link_libraries(${PROJECT_NAME} netcdf_built)
    set(AnEnIO_RPATH "${AnEnIO_RPATH};${install_dir}/install/lib")

else (NETCDF_BUILT)
    target_link_libraries (${PROJECT_NAME} ${NETCDF_LIBRARIES})
endif (NETCDF_BUILT)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
