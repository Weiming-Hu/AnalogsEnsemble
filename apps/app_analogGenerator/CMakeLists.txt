cmake_minimum_required(VERSION 2.8)
project(analogGenerator)

set (analogGenerator_VERSION "3.0.0")

enable_language(C CXX)
message(STATUS "build application Analog Generator")
set(SOURCE_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/analogGenerator.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_MAIN})

include_directories(SYSTEM ${NETCDF_INCLUDES})
include_directories(SYSTEM ${NETCDF_CXX4_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
add_dependencies(${PROJECT_NAME} AnEnIO)
target_link_libraries(${PROJECT_NAME} AnEnIO)

include_directories("${CMAKE_SOURCE_DIR}/CAnEn/include")
add_dependencies(${PROJECT_NAME} AnEn)
target_link_libraries(${PROJECT_NAME} AnEn)

set_target_properties(analogGenerator
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")
install(TARGETS analogGenerator RUNTIME DESTINATION bin)
