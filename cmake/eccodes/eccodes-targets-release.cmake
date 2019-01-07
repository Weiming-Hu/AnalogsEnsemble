#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "eccodes_memfs" for configuration "Release"
set_property(TARGET eccodes_memfs APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(eccodes_memfs PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/lib/x86_64-linux-gnu/libeccodes_memfs.so.0"
  IMPORTED_SONAME_RELEASE "libeccodes_memfs.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS eccodes_memfs )
list(APPEND _IMPORT_CHECK_FILES_FOR_eccodes_memfs "/usr/lib/x86_64-linux-gnu/libeccodes_memfs.so.0" )

# Import target "eccodes_memfs_static" for configuration "Release"
set_property(TARGET eccodes_memfs_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(eccodes_memfs_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "/usr/lib/x86_64-linux-gnu/libeccodes_memfs_static.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS eccodes_memfs_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_eccodes_memfs_static "/usr/lib/x86_64-linux-gnu/libeccodes_memfs_static.a" )

# Import target "eccodes" for configuration "Release"
set_property(TARGET eccodes APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(eccodes PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/usr/lib/x86_64-linux-gnu/libpng.so;/usr/lib/x86_64-linux-gnu/libz.so;/usr/lib/x86_64-linux-gnu/libaec.so;/usr/lib/x86_64-linux-gnu/libm.so;/usr/lib/x86_64-linux-gnu/libopenjp2.so;eccodes_memfs;-lpthread"
  IMPORTED_LOCATION_RELEASE "/usr/lib/x86_64-linux-gnu/libeccodes.so.0"
  IMPORTED_SONAME_RELEASE "libeccodes.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS eccodes )
list(APPEND _IMPORT_CHECK_FILES_FOR_eccodes "/usr/lib/x86_64-linux-gnu/libeccodes.so.0" )

# Import target "eccodes_static" for configuration "Release"
set_property(TARGET eccodes_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(eccodes_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/usr/lib/x86_64-linux-gnu/libpng.so;/usr/lib/x86_64-linux-gnu/libz.so;/usr/lib/x86_64-linux-gnu/libaec.so;/usr/lib/x86_64-linux-gnu/libm.so;/usr/lib/x86_64-linux-gnu/libopenjp2.so;eccodes_memfs;-lpthread"
  IMPORTED_LOCATION_RELEASE "/usr/lib/x86_64-linux-gnu/libeccodes_static.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS eccodes_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_eccodes_static "/usr/lib/x86_64-linux-gnu/libeccodes_static.a" )

# Import target "codes_info" for configuration "Release"
set_property(TARGET codes_info APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(codes_info PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/codes_info"
  )

list(APPEND _IMPORT_CHECK_TARGETS codes_info )
list(APPEND _IMPORT_CHECK_FILES_FOR_codes_info "/usr/bin/codes_info" )

# Import target "codes_count" for configuration "Release"
set_property(TARGET codes_count APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(codes_count PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/codes_count"
  )

list(APPEND _IMPORT_CHECK_TARGETS codes_count )
list(APPEND _IMPORT_CHECK_FILES_FOR_codes_count "/usr/bin/codes_count" )

# Import target "codes_split_file" for configuration "Release"
set_property(TARGET codes_split_file APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(codes_split_file PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/codes_split_file"
  )

list(APPEND _IMPORT_CHECK_TARGETS codes_split_file )
list(APPEND _IMPORT_CHECK_FILES_FOR_codes_split_file "/usr/bin/codes_split_file" )

# Import target "grib_histogram" for configuration "Release"
set_property(TARGET grib_histogram APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_histogram PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_histogram"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_histogram )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_histogram "/usr/bin/grib_histogram" )

# Import target "grib_filter" for configuration "Release"
set_property(TARGET grib_filter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_filter PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_filter"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_filter )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_filter "/usr/bin/grib_filter" )

# Import target "grib_ls" for configuration "Release"
set_property(TARGET grib_ls APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_ls PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_ls"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_ls )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_ls "/usr/bin/grib_ls" )

# Import target "grib_dump" for configuration "Release"
set_property(TARGET grib_dump APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_dump PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_dump"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_dump )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_dump "/usr/bin/grib_dump" )

# Import target "grib_merge" for configuration "Release"
set_property(TARGET grib_merge APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_merge PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_merge"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_merge )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_merge "/usr/bin/grib_merge" )

# Import target "grib2ppm" for configuration "Release"
set_property(TARGET grib2ppm APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib2ppm PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib2ppm"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib2ppm )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib2ppm "/usr/bin/grib2ppm" )

# Import target "grib_set" for configuration "Release"
set_property(TARGET grib_set APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_set PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_set"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_set )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_set "/usr/bin/grib_set" )

# Import target "grib_get" for configuration "Release"
set_property(TARGET grib_get APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_get PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_get"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_get )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_get "/usr/bin/grib_get" )

# Import target "grib_get_data" for configuration "Release"
set_property(TARGET grib_get_data APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_get_data PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_get_data"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_get_data )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_get_data "/usr/bin/grib_get_data" )

# Import target "grib_copy" for configuration "Release"
set_property(TARGET grib_copy APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_copy PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_copy"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_copy )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_copy "/usr/bin/grib_copy" )

# Import target "grib_compare" for configuration "Release"
set_property(TARGET grib_compare APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_compare PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_compare"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_compare )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_compare "/usr/bin/grib_compare" )

# Import target "codes_parser" for configuration "Release"
set_property(TARGET codes_parser APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(codes_parser PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/codes_parser"
  )

list(APPEND _IMPORT_CHECK_TARGETS codes_parser )
list(APPEND _IMPORT_CHECK_FILES_FOR_codes_parser "/usr/bin/codes_parser" )

# Import target "grib_index_build" for configuration "Release"
set_property(TARGET grib_index_build APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_index_build PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_index_build"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_index_build )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_index_build "/usr/bin/grib_index_build" )

# Import target "bufr_ls" for configuration "Release"
set_property(TARGET bufr_ls APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bufr_ls PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/bufr_ls"
  )

list(APPEND _IMPORT_CHECK_TARGETS bufr_ls )
list(APPEND _IMPORT_CHECK_FILES_FOR_bufr_ls "/usr/bin/bufr_ls" )

# Import target "bufr_dump" for configuration "Release"
set_property(TARGET bufr_dump APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bufr_dump PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/bufr_dump"
  )

list(APPEND _IMPORT_CHECK_TARGETS bufr_dump )
list(APPEND _IMPORT_CHECK_FILES_FOR_bufr_dump "/usr/bin/bufr_dump" )

# Import target "bufr_set" for configuration "Release"
set_property(TARGET bufr_set APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bufr_set PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/bufr_set"
  )

list(APPEND _IMPORT_CHECK_TARGETS bufr_set )
list(APPEND _IMPORT_CHECK_FILES_FOR_bufr_set "/usr/bin/bufr_set" )

# Import target "bufr_get" for configuration "Release"
set_property(TARGET bufr_get APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bufr_get PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/bufr_get"
  )

list(APPEND _IMPORT_CHECK_TARGETS bufr_get )
list(APPEND _IMPORT_CHECK_FILES_FOR_bufr_get "/usr/bin/bufr_get" )

# Import target "bufr_copy" for configuration "Release"
set_property(TARGET bufr_copy APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bufr_copy PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/bufr_copy"
  )

list(APPEND _IMPORT_CHECK_TARGETS bufr_copy )
list(APPEND _IMPORT_CHECK_FILES_FOR_bufr_copy "/usr/bin/bufr_copy" )

# Import target "bufr_compare" for configuration "Release"
set_property(TARGET bufr_compare APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bufr_compare PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/bufr_compare"
  )

list(APPEND _IMPORT_CHECK_TARGETS bufr_compare )
list(APPEND _IMPORT_CHECK_FILES_FOR_bufr_compare "/usr/bin/bufr_compare" )

# Import target "bufr_index_build" for configuration "Release"
set_property(TARGET bufr_index_build APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bufr_index_build PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/bufr_index_build"
  )

list(APPEND _IMPORT_CHECK_TARGETS bufr_index_build )
list(APPEND _IMPORT_CHECK_FILES_FOR_bufr_index_build "/usr/bin/bufr_index_build" )

# Import target "gts_get" for configuration "Release"
set_property(TARGET gts_get APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gts_get PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/gts_get"
  )

list(APPEND _IMPORT_CHECK_TARGETS gts_get )
list(APPEND _IMPORT_CHECK_FILES_FOR_gts_get "/usr/bin/gts_get" )

# Import target "gts_compare" for configuration "Release"
set_property(TARGET gts_compare APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gts_compare PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/gts_compare"
  )

list(APPEND _IMPORT_CHECK_TARGETS gts_compare )
list(APPEND _IMPORT_CHECK_FILES_FOR_gts_compare "/usr/bin/gts_compare" )

# Import target "gts_copy" for configuration "Release"
set_property(TARGET gts_copy APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gts_copy PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/gts_copy"
  )

list(APPEND _IMPORT_CHECK_TARGETS gts_copy )
list(APPEND _IMPORT_CHECK_FILES_FOR_gts_copy "/usr/bin/gts_copy" )

# Import target "gts_dump" for configuration "Release"
set_property(TARGET gts_dump APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gts_dump PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/gts_dump"
  )

list(APPEND _IMPORT_CHECK_TARGETS gts_dump )
list(APPEND _IMPORT_CHECK_FILES_FOR_gts_dump "/usr/bin/gts_dump" )

# Import target "gts_filter" for configuration "Release"
set_property(TARGET gts_filter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gts_filter PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/gts_filter"
  )

list(APPEND _IMPORT_CHECK_TARGETS gts_filter )
list(APPEND _IMPORT_CHECK_FILES_FOR_gts_filter "/usr/bin/gts_filter" )

# Import target "gts_ls" for configuration "Release"
set_property(TARGET gts_ls APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(gts_ls PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/gts_ls"
  )

list(APPEND _IMPORT_CHECK_TARGETS gts_ls )
list(APPEND _IMPORT_CHECK_FILES_FOR_gts_ls "/usr/bin/gts_ls" )

# Import target "metar_dump" for configuration "Release"
set_property(TARGET metar_dump APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(metar_dump PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/metar_dump"
  )

list(APPEND _IMPORT_CHECK_TARGETS metar_dump )
list(APPEND _IMPORT_CHECK_FILES_FOR_metar_dump "/usr/bin/metar_dump" )

# Import target "metar_ls" for configuration "Release"
set_property(TARGET metar_ls APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(metar_ls PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/metar_ls"
  )

list(APPEND _IMPORT_CHECK_TARGETS metar_ls )
list(APPEND _IMPORT_CHECK_FILES_FOR_metar_ls "/usr/bin/metar_ls" )

# Import target "metar_compare" for configuration "Release"
set_property(TARGET metar_compare APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(metar_compare PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/metar_compare"
  )

list(APPEND _IMPORT_CHECK_TARGETS metar_compare )
list(APPEND _IMPORT_CHECK_FILES_FOR_metar_compare "/usr/bin/metar_compare" )

# Import target "metar_get" for configuration "Release"
set_property(TARGET metar_get APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(metar_get PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/metar_get"
  )

list(APPEND _IMPORT_CHECK_TARGETS metar_get )
list(APPEND _IMPORT_CHECK_FILES_FOR_metar_get "/usr/bin/metar_get" )

# Import target "metar_filter" for configuration "Release"
set_property(TARGET metar_filter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(metar_filter PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/metar_filter"
  )

list(APPEND _IMPORT_CHECK_TARGETS metar_filter )
list(APPEND _IMPORT_CHECK_FILES_FOR_metar_filter "/usr/bin/metar_filter" )

# Import target "metar_copy" for configuration "Release"
set_property(TARGET metar_copy APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(metar_copy PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/metar_copy"
  )

list(APPEND _IMPORT_CHECK_TARGETS metar_copy )
list(APPEND _IMPORT_CHECK_FILES_FOR_metar_copy "/usr/bin/metar_copy" )

# Import target "grib_count" for configuration "Release"
set_property(TARGET grib_count APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_count PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_count"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_count )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_count "/usr/bin/grib_count" )

# Import target "bufr_count" for configuration "Release"
set_property(TARGET bufr_count APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(bufr_count PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/bufr_count"
  )

list(APPEND _IMPORT_CHECK_TARGETS bufr_count )
list(APPEND _IMPORT_CHECK_FILES_FOR_bufr_count "/usr/bin/bufr_count" )

# Import target "grib_to_netcdf" for configuration "Release"
set_property(TARGET grib_to_netcdf APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(grib_to_netcdf PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/grib_to_netcdf"
  )

list(APPEND _IMPORT_CHECK_TARGETS grib_to_netcdf )
list(APPEND _IMPORT_CHECK_FILES_FOR_grib_to_netcdf "/usr/bin/grib_to_netcdf" )

# Import target "codes_bufr_filter" for configuration "Release"
set_property(TARGET codes_bufr_filter APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(codes_bufr_filter PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/codes_bufr_filter"
  )

list(APPEND _IMPORT_CHECK_TARGETS codes_bufr_filter )
list(APPEND _IMPORT_CHECK_FILES_FOR_codes_bufr_filter "/usr/bin/codes_bufr_filter" )

# Import target "eccodes_f90" for configuration "Release"
set_property(TARGET eccodes_f90 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(eccodes_f90 PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "eccodes"
  IMPORTED_LOCATION_RELEASE "/usr/lib/x86_64-linux-gnu/libeccodes_f90.so.0"
  IMPORTED_SONAME_RELEASE "libeccodes_f90.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS eccodes_f90 )
list(APPEND _IMPORT_CHECK_FILES_FOR_eccodes_f90 "/usr/lib/x86_64-linux-gnu/libeccodes_f90.so.0" )

# Import target "eccodes_f90_static" for configuration "Release"
set_property(TARGET eccodes_f90_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(eccodes_f90_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;Fortran"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "eccodes"
  IMPORTED_LOCATION_RELEASE "/usr/lib/x86_64-linux-gnu/libeccodes_f90_static.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS eccodes_f90_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_eccodes_f90_static "/usr/lib/x86_64-linux-gnu/libeccodes_f90_static.a" )

# Import target "tigge_check" for configuration "Release"
set_property(TARGET tigge_check APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tigge_check PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/tigge_check"
  )

list(APPEND _IMPORT_CHECK_TARGETS tigge_check )
list(APPEND _IMPORT_CHECK_FILES_FOR_tigge_check "/usr/bin/tigge_check" )

# Import target "tigge_name" for configuration "Release"
set_property(TARGET tigge_name APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tigge_name PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/tigge_name"
  )

list(APPEND _IMPORT_CHECK_TARGETS tigge_name )
list(APPEND _IMPORT_CHECK_FILES_FOR_tigge_name "/usr/bin/tigge_name" )

# Import target "tigge_accumulations" for configuration "Release"
set_property(TARGET tigge_accumulations APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tigge_accumulations PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/tigge_accumulations"
  )

list(APPEND _IMPORT_CHECK_TARGETS tigge_accumulations )
list(APPEND _IMPORT_CHECK_FILES_FOR_tigge_accumulations "/usr/bin/tigge_accumulations" )

# Import target "tigge_split" for configuration "Release"
set_property(TARGET tigge_split APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tigge_split PROPERTIES
  IMPORTED_LOCATION_RELEASE "/usr/bin/tigge_split"
  )

list(APPEND _IMPORT_CHECK_TARGETS tigge_split )
list(APPEND _IMPORT_CHECK_FILES_FOR_tigge_split "/usr/bin/tigge_split" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
