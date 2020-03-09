###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################

# This file builds the utility anen_grib. This target depends on targets AnEn and AnEnIO.

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(anen_grib VERSION ${GRAND_VERSION} LANGUAGES CXX)
message(STATUS "Configuring the executable ${PROJECT_NAME}")

# Find the dependent libraries
find_package(AnEnIO)
find_package(Boost 1.58.0 REQUIRED COMPONENTS program_options)

# Create target
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/anen_grib.cpp)

# Configure the properties of this target
target_link_libraries(${PROJECT_NAME} PRIVATE AnEnIO::AnEnIO Boost::program_options)

# Export the executable
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets RUNTIME DESTINATION bin)


############################################################################################
#                                 Add executable anen_grib_mpi                             #
############################################################################################

if(ENABLE_MPI)
    message(STATUS "Build the class the executable ${PROJECT_NAME}_mpi")
    find_package(AnEnIOMPI)

    # Add executable
    add_executable(${PROJECT_NAME}_mpi ${CMAKE_CURRENT_SOURCE_DIR}/anen_grib.cpp)
    target_compile_definitions(${PROJECT_NAME}_mpi PRIVATE -D_USE_MPI_EXTENSION)

    target_link_libraries(${PROJECT_NAME}_mpi PRIVATE
        AnEnIOMPI::AnEnIOMPI Boost::program_options)

    install(TARGETS ${PROJECT_NAME}_mpi EXPORT
        ${PROJECT_NAME}_mpiTargets RUNTIME DESTINATION bin)

endif(MPI_CXX_FOUND)

