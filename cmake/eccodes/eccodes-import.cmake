if( ECCODES_IS_BUILD_DIR_EXPORT )
  set( ECCODES_DEFINITION_PATH  /build/eccodes/debian/build/share/eccodes/definitions )
  set( ECCODES_SAMPLES_PATH     /build/eccodes/debian/build/share/eccodes/samples )
  set( ECCODES_IFS_SAMPLES_PATH /build/eccodes/debian/build/share/eccodes/ifs_samples )
else()
  set( ECCODES_DEFINITION_PATH  /usr/share/eccodes/definitions )
  set( ECCODES_SAMPLES_PATH     /usr/share/eccodes/samples )
  set( ECCODES_IFS_SAMPLES_PATH /usr/share/eccodes/ifs_samples )
endif()
