cmake_minimum_required(VERSION 2.8)
project(gribConverter)

set (gribConverter_VERSION "1.0.0")

find_package(eccodes)
if(eccodes_FOUND)
    message(STATUS "Build application Grib Converter.")
    message(STATUS "Eccodes include dir is found at ${ECCODES_INCLUDE_DIRS}.")

    string(FIND "${ECCODES_FEATURES}" "ECCODES_THREADS" PTHREADS_POS)
    string(FIND "${ECCODES_FEATURES}" "ECCODES_OMP_THREADS" OMP_POS)

    if (${PTHREADS_POS} STREQUAL "-1" AND ${OMP_POS} STREQUAL "-1")
        message(FATAL_ERROR "Required feature not found: ECCODES_THREADS or ECCODES_OMP_THREADS.")
    endif (${PTHREADS_POS} STREQUAL "-1" AND ${OMP_POS} STREQUAL "-1")

    if (NOT ${PTHREADS_POS} STREQUAL "-1")
        message(STATUS "Required feature found: ECCODES_THREADS")
    endif (NOT ${PTHREADS_POS} STREQUAL "-1")

    if (NOT ${OMP_POS} STREQUAL "-1")
        message(STATUS "Required feature found: ECCODES_OMP_THREADS")
    endif (NOT ${OMP_POS} STREQUAL "-1")

else(eccodes_FOUND)
    message(STATUS "Eccodes library is not found. gribConverter is NOT built.")
    return()
endif(eccodes_FOUND)

enable_language(C CXX)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCE_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/gribConverter.cpp;${CMAKE_CURRENT_SOURCE_DIR}/gribConverterFunctions.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_MAIN})
#set(SOURCE_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/gribConverterFunctions.cpp")
#add_library(${PROJECT_NAME} SHARED ${SOURCE_MAIN})

include_directories(SYSTEM ${NETCDF_INCLUDES})
include_directories(SYSTEM ${NETCDF_CXX4_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
add_dependencies(${PROJECT_NAME} AnEnIO)
target_link_libraries(${PROJECT_NAME} AnEnIO)

include_directories("${CMAKE_SOURCE_DIR}/CAnEn/include")
add_dependencies(${PROJECT_NAME} AnEn)
target_link_libraries(${PROJECT_NAME} AnEn)

include_directories(${ECCODES_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} eccodes)

if(BOOST_TYPE STREQUAL "BUILD")
    target_link_libraries (${PROJECT_NAME} datetime)
endif(BOOST_TYPE STREQUAL "BUILD")

#add_executable(gribConverter "${CMAKE_CURRENT_SOURCE_DIR}/gribConverter.cpp")
#target_link_libraries(gribConverter ${PROJECT_NAME})
set_target_properties(gribConverter
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")
install(TARGETS gribConverter RUNTIME DESTINATION bin)
