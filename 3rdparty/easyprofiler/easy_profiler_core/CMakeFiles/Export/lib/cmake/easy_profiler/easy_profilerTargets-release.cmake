#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "easy_profiler" for configuration "Release"
set_property(TARGET easy_profiler APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(easy_profiler PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libeasy_profiler.so"
  IMPORTED_SONAME_RELEASE "libeasy_profiler.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS easy_profiler )
list(APPEND _IMPORT_CHECK_FILES_FOR_easy_profiler "${_IMPORT_PREFIX}/bin/libeasy_profiler.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
