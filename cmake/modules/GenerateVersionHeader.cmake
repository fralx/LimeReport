find_package(Git)

if(GIT_EXECUTABLE)
  # Generate a git-describe version string from Git repository tags
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
    RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  if(NOT GIT_DESCRIBE_ERROR_CODE)
    set(GIT_VERSION ${GIT_DESCRIBE_VERSION})
  endif()
endif()

# Final fallback: Just use a bogus version string that is semantically older
# than anything else and spit out a warning to the developer.
if(NOT DEFINED GIT_VERSION)
  set(GIT_VERSION 0.0.0-unknown)
  message(WARNING "Failed to determine version from Git tags. Using default version \"${GIT_VERSION}\".")
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/limereport/version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/limereport/version.h
  @ONLY)

