# - Find eccodes
# Find eccodes includes and libraries given ECCODES_DIR
#
#  ECCODES_INCLUDES  - where to find eccodes.h, etc
#  ECCODES_LIBRARIES - Link these libraries when using eccodes
#  ECCODES_FOUND     - True if eccodes found including required interfaces (see below)
#
# You can require certain interfaces via COMPONENTS: C, Fortran
#
# The following are not for general use and are included in
# ECCODES_LIBRARIES if the corresponding option above is set.
#
#  ECCODES_LIB_C   - Just the C interface
#  ECCODES_LIB_F90 - Fortran 90 interface, if available
#
# Normal usage would be:
#  find_package (eccodes COMPONENTS Fortran REQUIRED)
#  target_link_libraries (uses_f90_interface ${ECCODES_LIB_F90})
#  target_link_libraries (only_uses_c_interface ${ECCODES_LIB_C})

find_path (ECCODES_INCLUDES eccodes.h HINTS ECCODES_DIR ENV ECCODES_DIR PATH_SUFFIXES "" "include")
if (ECCODES_INCLUDES)
    set (ECCODES_INCLUDE_DIRS "${ECCODES_INCLUDES}")
endif()

find_library (ECCODES_LIB_C NAMES eccodes HINTS ECCODES_DIR ENV ECCODES_DIR PATH_SUFFIXES "" "lib")
mark_as_advanced (ECCODES_LIB_C)

set (eccodes_has_interfaces "YES") # will be set to NO if we're missing any interfaces
set (eccodes_libs "${ECCODES_LIB_C}")

get_filename_component (eccodes_lib_dirs "${ECCODES_LIB_C}" PATH)

macro (eccodes_check_interface lang header libs)
    find_path (ECCODES_INCLUDES_${lang} NAMES ${header} HINTS "${ECCODES_INCLUDES}" NO_DEFAULT_PATH)
    find_library (ECCODES_LIB_${lang} NAMES ${libs} HINTS "${eccodes_lib_dirs}" NO_DEFAULT_PATH)
    mark_as_advanced (ECCODES_INCLUDES_${lang} ECCODES_LIB_${lang})
    if (ECCODES_INCLUDES_${lang} AND ECCODES_LIB_${lang})
        list (INSERT eccodes_libs 0 ${ECCODES_LIB_${lang}}) # prepend so that -leccodes is last
    else()
        set (eccodes_has_interfaces "NO")
        message (STATUS "Failed to find ecCodes interface for ${lang}")
    endif()
endmacro()

if (eccodes_FIND_COMPONENTS)
    foreach (component IN LISTS eccodes_FIND_COMPONENTS)
        if (component STREQUAL "Fortran")
            eccodes_check_interface (F90 eccodes.mod eccodes_f90)
        elseif (component STREQUAL "C")
            eccodes_check_interface (C eccodes.h eccodes)
        endif()
    endforeach()
else()
    eccodes_check_interface (C eccodes.h eccodes)
endif()

list (REMOVE_DUPLICATES eccodes_libs)
set (ECCODES_LIBRARIES "${eccodes_libs}" CACHE STRING "ecCodes libraries")

# handle the QUIETLY and REQUIRED arguments and set ECCODES_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (eccodes DEFAULT_MSG ECCODES_LIBRARIES ECCODES_INCLUDES eccodes_has_interfaces)

mark_as_advanced (ECCODES_LIBRARIES ECCODES_INCLUDES)
