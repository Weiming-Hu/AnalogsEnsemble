cmake_minimum_required(VERSION 2.8)
project(forecastsToObservations)

add_definitions(-D_APPVERSION="v 1.0.2")

enable_language(C CXX)
message(STATUS "build application Forecasts to Observations")
set(SOURCE_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/forecastsToObservations.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_MAIN})

include_directories(SYSTEM ${NETCDF_INCLUDES})
include_directories(SYSTEM ${NETCDF_CXX4_INCLUDE_DIR})
include_directories("${CMAKE_SOURCE_DIR}/CAnEnIO/include")
add_dependencies(${PROJECT_NAME} AnEnIO)
target_link_libraries(${PROJECT_NAME} AnEnIO)

include_directories("${CMAKE_SOURCE_DIR}/CAnEn/include")
add_dependencies(${PROJECT_NAME} AnEn)
target_link_libraries(${PROJECT_NAME} AnEn)

if(${ENABLE_MPI})
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
    target_link_libraries(${PROJECT_NAME} ${MPI_CXX_LIBRARIES})
    if(MPI_CXX_COMPILE_FLAGS)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            COMPILE_FLAGS ${MPI_CXX_COMPILE_FLAGS})
    endif(MPI_CXX_COMPILE_FLAGS)
    if(MPI_CXX_LINK_FLAGS)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS ${MPI_CXX_LINK_FLAGS})
    endif(MPI_CXX_LINK_FLAGS)
endif(${ENABLE_MPI}) 

set_target_properties(forecastsToObservations
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")
install(TARGETS forecastsToObservations RUNTIME DESTINATION bin)
