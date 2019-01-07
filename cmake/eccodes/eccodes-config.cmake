# Config file for the eccodes package
# Defines the following variables:
#
#  ECCODES_INCLUDE_DIRS   - include directories
#  ECCODES_DEFINITIONS    - preprocessor definitions
#  ECCODES_LIBRARIES      - libraries to link against
#  ECCODES_FEATURES       - list of enabled features
#  ECCODES_VERSION        - version of the package
#  ECCODES_GIT_SHA1       - Git revision of the package
#  ECCODES_GIT_SHA1_SHORT - short Git revision of the package
#
# Also defines eccodes third-party library dependencies:
#  ECCODES_TPLS             - package names of  third-party library dependencies
#  ECCODES_TPL_INCLUDE_DIRS - include directories
#  ECCODES_TPL_DEFINITIONS  - preprocessor definitions
#  ECCODES_TPL_LIBRARIES    - libraries to link against

### compute paths

get_filename_component(ECCODES_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set( ECCODES_SELF_INCLUDE_DIRS "${ECCODES_CMAKE_DIR}/../../../include" )
set( ECCODES_SELF_DEFINITIONS  "" )
set( ECCODES_SELF_LIBRARIES    "eccodes;eccodes_f90" )

set( ECCODES_TPLS              "PNG;AEC;Python;NumPy;PythonInterp;CMath;OpenJPEG" )
set( ECCODES_TPL_INCLUDE_DIRS  "/usr/include;/usr/lib/python3/dist-packages/numpy/core/include;/usr/include/openjpeg-2.1" )
set( ECCODES_TPL_DEFINITIONS   "" )
set( ECCODES_TPL_LIBRARIES     "/usr/lib/x86_64-linux-gnu/libpng.so;/usr/lib/x86_64-linux-gnu/libz.so;/usr/lib/x86_64-linux-gnu/libaec.so;/usr/lib/x86_64-linux-gnu/libm.so;/usr/lib/x86_64-linux-gnu/libopenjp2.so" )

set( ECCODES_VERSION           "2.6.0" )
set( ECCODES_GIT_SHA1          "" )
set( ECCODES_GIT_SHA1_SHORT    "" )

### export include paths as absolute paths

set( ECCODES_INCLUDE_DIRS "" )
foreach( path ${ECCODES_SELF_INCLUDE_DIRS} )
  get_filename_component( abspath ${path} ABSOLUTE )
  list( APPEND ECCODES_INCLUDE_DIRS ${abspath} )
endforeach()
list( APPEND ECCODES_INCLUDE_DIRS ${ECCODES_TPL_INCLUDE_DIRS} )

### export definitions

set( ECCODES_DEFINITIONS      ${ECCODES_SELF_DEFINITIONS} ${ECCODES_TPL_DEFINITIONS} )

### export list of all libraries

set( ECCODES_LIBRARIES        ${ECCODES_SELF_LIBRARIES}   ${ECCODES_TPL_LIBRARIES}   )

### export the features provided by the package

set( ECCODES_FEATURES    "TESTS;EXAMPLES;JPG;PNG;NETCDF;AEC;PYTHON;FORTRAN;BIT_REPRODUCIBLE;MEMFS;INSTALL_ECCODES_DEFINITIONS;INSTALL_ECCODES_SAMPLES;ALIGN_MEMORY;GRIB_TIMER;ECCODES_OMP_THREADS" )
foreach( _f ${ECCODES_FEATURES} )
  set( ECCODES_HAVE_${_f} 1 )
endforeach()

# Has this configuration been exported from a build tree?
set( ECCODES_IS_BUILD_DIR_EXPORT OFF )

if( EXISTS ${ECCODES_CMAKE_DIR}/eccodes-import.cmake )
  set( ECCODES_IMPORT_FILE "${ECCODES_CMAKE_DIR}/eccodes-import.cmake" )
  include( ${ECCODES_IMPORT_FILE} )
endif()

# here goes the imports of the TPL's

include( ${CMAKE_CURRENT_LIST_FILE}.tpls OPTIONAL )

# insert definitions for IMPORTED targets

if( NOT eccodes_BINARY_DIR )

  if( ECCODES_IS_BUILD_DIR_EXPORT )
    include( "/build/eccodes/debian/build/eccodes-targets.cmake" OPTIONAL )
  else()
    include( "${ECCODES_CMAKE_DIR}/eccodes-targets.cmake" OPTIONAL )
  endif()

endif()

# publish this file as imported

set( ECCODES_IMPORT_FILE ${CMAKE_CURRENT_LIST_FILE} )
mark_as_advanced( ECCODES_IMPORT_FILE )

# set eccodes_BASE_DIR for final installations or build directories

if( NOT eccodes )
  if( ECCODES_IS_BUILD_DIR_EXPORT )
    set( eccodes_BASE_DIR /build/eccodes/debian/build )
  else()
    get_filename_component( abspath ${CMAKE_CURRENT_LIST_DIR}/../../.. ABSOLUTE )
    set( eccodes_BASE_DIR ${abspath} )
  endif()
endif()
