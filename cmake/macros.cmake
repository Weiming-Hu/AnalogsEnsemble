###################################################################################
# Author: Weiming Hu <weiming@psu.edu>                                            #
#         Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu) #
#         Department of Geography                                                 #
#         Institute for Computational and Data Science                            #
#         The Pennsylvania State University                                       #
###################################################################################
#
# This file includes some macros for the project
#


# Define a macro for find_package so that it works for our subprojects.
# If the package is in the list SUBPROJECT_TARGETS, this macro will simply 
# make the find_package into an no-op.
#
macro(find_package)
    if(NOT "${ARGV0}" IN_LIST SUBPROJECT_TARGETS)
        _find_package(${ARGV})
    endif(NOT "${ARGV0}" IN_LIST SUBPROJECT_TARGETS)
endmacro(find_package)


# Since this project dependends on boost-cmake which does not support installation
# yet, if boost was built, one should set the NO_INSTALLL_TARGETS to include the
# libraries that depend on the built boost so that they are not installed. But
# executables can be installed so they should not be included.
#
macro(install)
    if(NOT "${ARGV0}" IN_LIST NO_INSTALL_LIBRARIES)
        _install(${ARGV})
    endif(NOT "${ARGV0}" IN_LIST NO_INSTALL_LIBRARIES)
endmacro(install)

