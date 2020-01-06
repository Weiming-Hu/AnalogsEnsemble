cmake_minimum_required (VERSION 2.8)
project (AnEn)
enable_language(CXX C)
message(STATUS "install CAnEn library")
set (LIBRARY_NAME "AnEn")

file (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

option(PROGRESS_BAR "Build progress bar" ON)
if (PROGRESS_BAR)
    message(STATUS "Build progress bar")
else(PROGRESS_BAR)
    message(STATUS "No progress bar")
    add_definitions(-DNOPROGRESS)
endif (PROGRESS_BAR)

##################
# build and link #
##################
#add_library(${LIBRARY_NAME} SHARED ${SOURCES})
add_library(${LIBRARY_NAME} STATIC ${SOURCES})

add_dependencies(${LIBRARY_NAME} "AnEnIO")
include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
target_link_libraries(${LIBRARY_NAME} AnEnIO)

if(BOOST_TYPE STREQUAL "BUILD")
    target_link_libraries (${LIBRARY_NAME} "system")
    target_link_libraries (${LIBRARY_NAME} "filesystem")
    target_link_libraries (${LIBRARY_NAME} "program_options")
elseif(BOOST_TYPE STREQUAL "SYSTEM")
    target_link_libraries (${LIBRARY_NAME} ${Boost_LIBRARIES})
endif(BOOST_TYPE STREQUAL "BUILD")

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
set_target_properties(${LIBRARY_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib"  # if static 
    LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib") # if shared

# Copy library to destination
#install(TARGETS ${LIBRARY_NAME} LIBRARY DESTINATION lib)
install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION lib)

# Copy headers to destination
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)
