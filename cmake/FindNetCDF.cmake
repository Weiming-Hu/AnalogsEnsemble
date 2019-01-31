# - Find NetCDF
# referenced from
# https://github.com/jedbrown/cmake-modules/blob/master/FindNetCDF.cmake
#
# Find the native NetCDF includes and library
#
#  NETCDF_INCLUDES    - where to find netcdf.h, etc
#  NETCDF_LIBRARIES   - Link these libraries when using NetCDF
#  NETCDF_FOUND       - True if NetCDF found including required interfaces (see below)
#  NETCDF_CFLAGS      - The C flags for NetCDF if nc-config is found
#  NETCDF_LINKING     - The linking command to link to NetCDF if nc-config is found
#
# Your package can require certain interfaces to be FOUND by setting these
#
#  NETCDF_CXX         - require the C++ interface and link the C++ library
#
#  (The following two options have been removed)
#  NETCDF_F77         - require the F77 interface and link the fortran library
#  NETCDF_F90         - require the F90 interface and link the fortran library
#
# The following are not for general use and are included in
# NETCDF_LIBRARIES if the corresponding option above is set.
#
#  NETCDF_LIBRARIES_C    - Just the C interface
#  NETCDF_LIBRARIES_CXX  - C++ interface, if available
#  NETCDF_LIBRARIES_F77  - Fortran 77 interface, if available
#  NETCDF_LIBRARIES_F90  - Fortran 90 interface, if available
#
# Normal usage would be:
#  set (NETCDF_F90 "YES")
#  find_package (NetCDF REQUIRED)
#  target_link_libraries (uses_f90_interface ${NETCDF_LIBRARIES})
#  target_link_libraries (only_uses_c_interface ${NETCDF_LIBRARIES_C})

if (NETCDF_INCLUDES AND NETCDF_LIBRARIES)
  # Already in cache, be silent
  set (NETCDF_FIND_QUIETLY TRUE)
endif (NETCDF_INCLUDES AND NETCDF_LIBRARIES)

find_path (NETCDF_INCLUDES netcdf.h
  HINTS NETCDF_DIR ENV NETCDF_DIR)

find_program (NETCDF_NCCONFIG nc-config HINTS NETCDF_DIR ENV NETCDF_DIR)
if (NETCDF_NCCONFIG)
    message(STATUS "nc-config is found at ${NETCDF_NCCONFIG}. Use this program to define include path and libs")

    execute_process(COMMAND ${NETCDF_NCCONFIG} "--cflags" OUTPUT_VARIABLE NETCDF_CFLAGS)
    string(STRIP ${NETCDF_CFLAGS} NETCDF_CFLAGS)
    string (REPLACE " " ";" NETCDF_CFLAGS "${NETCDF_CFLAGS}")

    execute_process(COMMAND ${NETCDF_NCCONFIG} "--libs" OUTPUT_VARIABLE NETCDF_LINKING)
    string(STRIP ${NETCDF_LINKING} NETCDF_LINKING)
    string (REPLACE " " ";" NETCDF_LINKING "${NETCDF_LINKING}")

else (NETCDF_NCCONFIG)
    message(WARNING "nc-config is NOT found. I have to guess the location and there might be linking problem.")
    set(NETCDF_CFLAGS "")
    set(NETCDF_LINKING "")
endif (NETCDF_NCCONFIG)

find_library (NETCDF_LIBRARIES_C NAMES netcdf)
mark_as_advanced(NETCDF_LIBRARIES_C)

# Will be set to YES if we found any of the combination
set (NetCDF_has_interfaces "NO")
set (NetCDF_libs "${NETCDF_LIBRARIES_C}")

get_filename_component (NetCDF_lib_dirs "${NETCDF_LIBRARIES_C}" PATH)

macro (NetCDF_check_interface lang header libs)
  if (NETCDF_${lang})
    find_path (NETCDF_INCLUDES_${lang} NAMES ${header}
        #HINTS "${NETCDF_INCLUDES}" NO_DEFAULT_PATH)
        # Weiming: Search in all possible directories
      HINTS "${NETCDF_INCLUDES}")
    find_library (NETCDF_LIBRARIES_${lang} NAMES ${libs}
        # HINTS "${NetCDF_lib_dirs}" NO_DEFAULT_PATH)
        # Weiming: Search in all possible directories
      HINTS "${NetCDF_lib_dirs}")
    mark_as_advanced (NETCDF_INCLUDES_${lang} NETCDF_LIBRARIES_${lang})
    if (NETCDF_INCLUDES_${lang} AND NETCDF_LIBRARIES_${lang})
      list (INSERT NetCDF_libs 0 ${NETCDF_LIBRARIES_${lang}}) # prepend so that -lnetcdf is last
      set (NetCDF_has_interfaces "Yes")
      # else (NETCDF_INCLUDES_${lang} AND NETCDF_LIBRARIES_${lang})
      # message (STATUS "Failed to find NetCDF interface for ${lang}")
    endif (NETCDF_INCLUDES_${lang} AND NETCDF_LIBRARIES_${lang})
  endif (NETCDF_${lang})
endmacro (NetCDF_check_interface)

# NetCDF_check_interface (CXX netcdf netcdf_c++4)
# NetCDF_check_interface (CXX netcdfcpp.h netcdf-cxx4)
# NetCDF_check_interface (CXX netcdfcpp.h netcdf_c++)
if (NETCDF_CXX STREQUAL "YES")
    set (possible_include_names "netcdf;netcdfcpp.h")
    set (possible_library_names "netcdf_c++4;netcdf-cxx4")
    list(LENGTH possible_include_names num_possible_include_names)
    list(LENGTH possible_library_names num_possible_library_names)

    set(i_include "0")
    while (i_include LESS ${num_possible_include_names})
        set(i_library "0")
        while (i_library LESS ${num_possible_library_names})
            list(GET possible_include_names ${i_include} find_CXX_include)
            list(GET possible_library_names ${i_library} find_CXX_library)
            NetCDF_check_interface (CXX ${find_CXX_include} ${find_CXX_library})

            math(EXPR i_library "${i_library} + 1")
        endwhile (i_library LESS ${num_possible_library_names})

        math(EXPR i_include "${i_include} + 1")
    endwhile (i_include LESS ${num_possible_include_names})
endif (NETCDF_CXX STREQUAL "YES")

# NetCDF_check_interface (F77 netcdf.inc  netcdff)
# NetCDF_check_interface (F90 netcdf.mod  netcdff)

set (NETCDF_LIBRARIES "${NetCDF_libs}" CACHE STRING "All NetCDF libraries required for interface level")

# handle the QUIETLY and REQUIRED arguments and set NETCDF_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)

if (NETCDF_CXX STREQUAL "YES")
    find_package_handle_standard_args (NetCDF DEFAULT_MSG NETCDF_LIBRARIES NETCDF_INCLUDES NetCDF_has_interfaces)
else (NETCDF_CXX STREQUAL "YES")
    find_package_handle_standard_args (NetCDF DEFAULT_MSG NETCDF_LIBRARIES NETCDF_INCLUDES)
endif (NETCDF_CXX STREQUAL "YES")

mark_as_advanced (NETCDF_LIBRARIES NETCDF_INCLUDES NETCDF_CFLAGS NETCDF_LINKING)
