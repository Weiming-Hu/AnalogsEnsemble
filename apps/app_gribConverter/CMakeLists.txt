cmake_minimum_required(VERSION 2.8)
project(gribConverter)

set (gribConverter_VERSION "1.0.0")

enable_language(C CXX)
message(STATUS "build application GRIB Converter")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCE_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/gribConverter.cpp;${CMAKE_CURRENT_SOURCE_DIR}/gribConverterFunctions.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_MAIN})

include_directories(SYSTEM ${NETCDF_INCLUDES})
include_directories(SYSTEM ${NETCDF_CXX4_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
add_dependencies(${PROJECT_NAME} AnEnIO)
target_link_libraries(${PROJECT_NAME} AnEnIO)

include_directories("${CMAKE_SOURCE_DIR}/CAnEn/include")
add_dependencies(${PROJECT_NAME} AnEn)
target_link_libraries(${PROJECT_NAME} AnEn)

find_package(eccodes REQUIRED)
include_directories(${ECCODES_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} eccodes)
target_link_libraries (${PROJECT_NAME} datetime)

set_target_properties(gribConverter
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")
install(TARGETS gribConverter RUNTIME DESTINATION bin)
