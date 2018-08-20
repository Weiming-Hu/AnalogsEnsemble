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
add_library (${LIBRARY_NAME} STATIC ${SOURCES})

include_directories(${NETCDF_CXX4_INCLUDE_DIR})
add_dependencies(${LIBRARY_NAME} NETCDF_CXX4_PROJECT)
target_link_libraries(${LIBRARY_NAME} ${NETCDF_CXX4_LIBRARIES})

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

install(TARGETS ${LIBRARY_NAME} ARCHIVE
    DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
    DESTINATION .)
