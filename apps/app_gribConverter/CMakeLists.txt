cmake_minimum_required(VERSION 2.8)
project(gribConverter)

if (NOT ECCODES_TYPE)
    set(ECCODES_TYPE "BUILD" CACHE STRING
        "The type of Eccodes library that is going to be used. Should be either SYSTEM or BUILD.")
endif (NOT ECCODES_TYPE)
message (STATUS "ECCODES_TYPE is set to ${ECCODES_TYPE}")

message(STATUS "Build application Grib Converter.")
if (ECCODES_TYPE STREQUAL "SYSTEM")
    find_package(eccodes)
    if(eccodes_FOUND)
        message(STATUS "Eccodes include dir is found at ${ECCODES_INCLUDE_DIRS}.")

        string(FIND "${ECCODES_FEATURES}" "ECCODES_THREADS" PTHREADS_POS)
        string(FIND "${ECCODES_FEATURES}" "ECCODES_OMP_THREADS" OMP_POS)

        if (${PTHREADS_POS} STREQUAL "-1" AND ${OMP_POS} STREQUAL "-1")
            message(STATUS "ecCodes are single-threaded.")
        else (${PTHREADS_POS} STREQUAL "-1" AND ${OMP_POS} STREQUAL "-1")
            message(STATUS "Eccodes lib is multithreading.\nThis will cause problem because multi-field support is needed but ecCodes have known issues for it.\nSet ECCODES_TYPE to BUILD to build a different version of ecCodes.")
        endif (${PTHREADS_POS} STREQUAL "-1" AND ${OMP_POS} STREQUAL "-1")
    else(eccodes_FOUND)
        message(WARNING "Eccodes library is not found. Setting the type to BUILD.")
        set(ECCODES_TYPE "BUILD")
    endif(eccodes_FOUND)
endif (ECCODES_TYPE STREQUAL "SYSTEM")

if (ECCODES_TYPE STREQUAL "BUILD")
    message(STATUS "Building Eccodes library")
    include(ExternalProject)
    # This is copied from the Eccodes website
    # https://confluence.ecmwf.int/display/ECC/Releases
    #
    set(ECCODES_URL "https://confluence.ecmwf.int/download/attachments/45757960/eccodes-2.10.0-Source.tar.gz?api=v2")
    set(ECCODES_INSTALL_PREFIX "-DCMAKE_INSTALL_PREFIX=${DEPENDENCY_PATH}/install")
    set(ECCODES_BUILD_SHARED "-DBUILD_SHARED_LIBS=ON")
    set(ECCODES_DISABLE_PYTHON "-DENABLE_PYTHON=OFF")
    set(ECCODES_ROOT_PATH ${DEPENDENCY_PATH})
    set(ECCODES_CMAKE_ARGS ${ECCODES_DISABLE_PYTHON} ${ECCODES_INSTALL_PREFIX} ${ECCODES_BUILD_SHARED})

    if (CMAKE_PREFIX_PATH)
        # This if statement and the code have several purposes:
        # - If CMAKE_PREFIX_PATH is set, pass it to external project.
        # - Deal with CMAKE_PREFIX_PATH with multiple folders by changing the separator.
        #
        string(REPLACE ";" "|" ECCODES_ALT_CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}")
        set(ECCODES_PREFIX_PATH "-DCMAKE_PREFIX_PATH=${ECCODES_ALT_CMAKE_PREFIX_PATH}")
        set(ECCODES_CMAKE_ARGS ${ECCODES_CMAKE_ARGS} ${ECCODES_PREFIX_PATH})
    endif(CMAKE_PREFIX_PATH)


    if(VERBOSE)
        set(VERBOSE_DOWNLOAD 0)
        set(VERBOSE_CONFIGURE 0)
        set(VERBOSE_BUILD 0) 
        set(VERBOSE_INSTALL 0) 
        set(VERBOSE_TEST 0)
    else(VERBOSE)
        set(VERBOSE_DOWNLOAD 1)
        set(VERBOSE_CONFIGURE 1)
        set(VERBOSE_BUILD 1) 
        set(VERBOSE_INSTALL 1) 
        set(VERBOSE_TEST 1)
    endif(VERBOSE)
    ExternalProject_add(ECCODES_PROJECT
        PREFIX ${ECCODES_ROOT_PATH}
        URL ${ECCODES_URL}
        PATCH_COMMAND ""
        UPDATE_COMMAND ""
        LIST_SEPARATOR | # Use the alternate list separator
        CMAKE_ARGS ${ECCODES_CMAKE_ARGS}
        BUILD_COMMAND ${MAKE}
        LOG_TEST ${VERBOSE_TEST}
        LOG_DOWNLOAD ${VERBOSE_DOWNLOAD}
        LOG_CONFIGURE ${VERBOSE_CONFIGURE}
        LOG_BUILD ${VERBOSE_BUILD}
        LOG_INSTALL ${VERBOSE_INSTALL})

    set (ECCODES_INCLUDE_DIRS "${DEPENDENCY_PATH}/install/include" CACHE STRING "ECCODES include dirs")
endif (ECCODES_TYPE STREQUAL "BUILD")

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
if (ECCODES_TYPE STREQUAL "SYSTEM")
    target_link_libraries(${PROJECT_NAME} eccodes)
else (ECCODES_TYPE STREQUAL "SYSTEM")
    ExternalProject_Get_Property(ECCODES_PROJECT install_dir)
    add_library(eccodes_built SHARED IMPORTED)
    set_target_properties(eccodes_built PROPERTIES
        IMPORTED_LOCATION "${install_dir}/install/lib/${CMAKE_SHARED_LIBRARY_PREFIX}eccodes${CMAKE_SHARED_LIBRARY_SUFFIX}")
    add_dependencies(eccodes_built ECCODES_PROJECT)
    target_link_libraries(${PROJECT_NAME} eccodes_built)

endif (ECCODES_TYPE STREQUAL "SYSTEM")

if(BOOST_TYPE STREQUAL "BUILD")
    target_link_libraries (${PROJECT_NAME} datetime)
endif(BOOST_TYPE STREQUAL "BUILD")

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

#add_executable(gribConverter "${CMAKE_CURRENT_SOURCE_DIR}/gribConverter.cpp")
#target_link_libraries(gribConverter ${PROJECT_NAME})
set_target_properties(gribConverter
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${COMMON_OUTPUT_DIR}/bin")
install(TARGETS gribConverter RUNTIME DESTINATION bin)
