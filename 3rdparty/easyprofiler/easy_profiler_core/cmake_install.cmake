# Install script for directory: /home/alex/Work/C++Projects/easyprofiler/easy_profiler_core

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/easy_profiler" TYPE FILE FILES
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/generated/easy_profilerConfig.cmake"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/generated/easy_profilerConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/easy" TYPE FILE FILES
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/arbitrary_value.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/easy_net.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/easy_socket.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/profiler.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/reader.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/serialized_block.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/details/arbitrary_value_aux.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/details/arbitrary_value_public_types.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/details/easy_compiler_support.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/details/profiler_aux.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/details/profiler_colors.h"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/include/easy/details/profiler_public_types.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/LICENSE.MIT"
    "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/LICENSE.APACHE"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libeasy_profiler.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libeasy_profiler.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libeasy_profiler.so"
         RPATH "$ORIGIN")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "/home/alex/Work/C++Projects/easyprofiler/bin/libeasy_profiler.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libeasy_profiler.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libeasy_profiler.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libeasy_profiler.so"
         OLD_RPATH ":::::::"
         NEW_RPATH "$ORIGIN")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/libeasy_profiler.so")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/easy_profiler/easy_profilerTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/easy_profiler/easy_profilerTargets.cmake"
         "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/CMakeFiles/Export/lib/cmake/easy_profiler/easy_profilerTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/easy_profiler/easy_profilerTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/easy_profiler/easy_profilerTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/easy_profiler" TYPE FILE FILES "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/CMakeFiles/Export/lib/cmake/easy_profiler/easy_profilerTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/easy_profiler" TYPE FILE FILES "/home/alex/Work/C++Projects/easyprofiler/easy_profiler_core/CMakeFiles/Export/lib/cmake/easy_profiler/easy_profilerTargets-release.cmake")
  endif()
endif()

