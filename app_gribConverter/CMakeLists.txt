cmake_minimum_required(VERSION 2.8)
project(gribConverter)

set (gribConverter_VERSION "1.0.0")

enable_language(C CXX)
message(STATUS "build application GRIB Converter")
set(SOURCE_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/gribConverter.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_MAIN})

include_directories(${NETCDF_INCLUDES})
include_directories(${NETCDF_CXX4_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
add_dependencies(${PROJECT_NAME} AnEnIO)
target_link_libraries(${PROJECT_NAME} AnEnIO)

include_directories("${CMAKE_SOURCE_DIR}/CAnEn/include")
add_dependencies(${PROJECT_NAME} AnEn)
target_link_libraries(${PROJECT_NAME} AnEn)

find_package(eccodes REQUIRED)
target_link_libraries(${PROJECT_NAME} eccodes)

set_target_properties(gribConverter
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")
install(TARGETS gribConverter RUNTIME DESTINATION bin)
