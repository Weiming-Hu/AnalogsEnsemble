##########################################################################################
#  This file is a cmake script containing common settings for all systems
#
#  Copyright 2016 Marc Rautenhaus
#  Copyright 2016 Michael Kern
#
##########################################################################################

# Common install directories)
set (COMMON_INSTALL_DIRS
        /usr/local
        /usr
        /sw # Fink
        /opt/local # DarwinPorts
        /opt/csw # Blastwave
        /opt
        /home/local/sw/gl
        /home/local/sw/unidata
        /home/local/sw/qt
        /home/local/sw/qt5
        /home/local/sw/ecmwf
    )

if (UNIX)

    include(FindPkgConfig)

    # Use pkg_config to detect INCLUDE/LIBRARY directories
    function(use_pkg_config pkg_name)

        if (${${pgk_name}_FOUND})
            return()
        endif()

        set(PREFIX Package)
        pkg_search_module(${PREFIX} ${pkg_name})

        if (${PREFIX}_FOUND)
            message(STATUS "Found pgk_config file for package ${pkg_name}")
        endif()

        set(PKG_LIBRARY_DIRS ${${PREFIX}_LIBRARY_DIRS} PARENT_SCOPE)
        set(PKG_INCLUDE_DIRS ${${PREFIX}_INCLUDE_DIRS} PARENT_SCOPE)
    endfunction()

endif (UNIX)
