cmake_minimum_required (VERSION 2.8)
project (AnEn)
enable_language(CXX C)
message(STATUS "install CAnEn library")
set (LIBRARY_NAME "AnEn")

file (GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

##################
# build and link #
##################
#add_library(${LIBRARY_NAME} SHARED ${SOURCES})
add_library(${LIBRARY_NAME} STATIC ${SOURCES})

add_dependencies(${LIBRARY_NAME} "AnEnIO")
include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
target_link_libraries(${LIBRARY_NAME} AnEnIO)

if(BOOST_TYPE STREQUAL "BUILD")
    add_dependencies(${LIBRARY_NAME} CMAKE_BOOST)
    target_link_libraries (${LIBRARY_NAME} CMAKE_BOOST_DATE_TIME)
elseif(BOOST_TYPE STREQUAL "SYSTEM")
    target_link_libraries (${LIBRARY_NAME} ${Boost_LIBRARIES})
endif(BOOST_TYPE STREQUAL "BUILD")

# Set default make output folder
set_target_properties(${LIBRARY_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib"  # if static 
    LIBRARY_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/lib") # if shared

# Copy library to destination
#install(TARGETS ${LIBRARY_NAME} LIBRARY DESTINATION lib)
install(TARGETS ${LIBRARY_NAME} ARCHIVE DESTINATION lib)

# Copy headers to destination
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION .)
