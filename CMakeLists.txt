project(marble)


####################################################
# CMake Settings 
SET(CMAKE_COLOR_MAKEFILE ON)



####################################################
# User configurable options
if(APPLE OR WIN32) #by default build Qt only version for OSX and win
  set (QTONLY TRUE CACHE BOOL "Determines if we should compile for Qt only.")
else(APPLE OR WIN32)
  set (QTONLY FALSE CACHE BOOL "Determines if we should compile for Qt only.")
endif(APPLE OR WIN32)
set (PEDANTIC FALSE CACHE BOOL "Determines if we should compile with -Wall -Werror.")
set (ENABLE_TESTS FALSE CACHE BOOL "Build unit tests?")
set (WITH_DESIGNER_PLUGIN FALSE CACHE BOOL "Build designer plugin?")
# @TODO - make os specific versions of below
set (MARBLE_DATA_PATH ${CMAKE_INSTALL_PREFIX}/share/marble CACHE STRING "Marble data directory")


if(WIN32)
  set (STATIC_BUILD false cache bool "Link to static Qt libs (win32 only)?")
endif(WIN32)

# Variables to test new kml implementation
# Will removed in feature after success
#add_definitions(-DKML_DEBUG)
#add_definitions(-DKML_GSOC)

####################################################
# Where to look first for cmake modules, 
# before ${CMAKE_ROOT}/Modules/ is checked
set ( CMAKE_MODULE_PATH 
     ${CMAKE_SOURCE_DIR}/cmake/modules 
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/src
     ${CMAKE_CURRENT_SOURCE_DIR}/cmake_find_rules
     )

####################################################
# Enable ctest
IF (ENABLE_TESTS)
  ENABLE_TESTING()
ENDIF (ENABLE_TESTS)

####################################################
# Options for static build
if(STATIC_BUILD)
  add_definitions(-DSTATIC_BUILD=1)
endif(STATIC_BUILD)

####################################################
# Workaround FindQt4.cmake bug not finding 
# QtDesigner includes
if(APPLE)
  if (NOT DEFINED ${QT_QTDESIGNER_INCLUDE_DIR})
    set ( QT_QTDESIGNER_INCLUDE_DIR 
    ${QT_LIBRARY_DIR}/QtDesigner.framework/Headers 
    )
  endif(NOT DEFINED ${QT_QTDESIGNER_INCLUDE_DIR})
endif(APPLE)

####################################################
# Specific options for if building with Qt or kde4 libs
if(QTONLY)
  find_package(Qt4 REQUIRED)
  # add a flag to be able to distinguish between qt 
  # and kde mode in the sources
  add_definitions(-DQTONLY)
else(QTONLY)
  # search packages used by KDE
  find_package(KDE4 REQUIRED)
  include (KDE4Defaults)
  include (MacroLibrary)
endif (QTONLY)

#############################################################
# Create marblesvnversion.h
# see http://public.kitware.com/pipermail/cmake/2006-July/010299.html
# We only do this if we are in a .svn dir

FIND_FILE(SVN_MARKER entries PATHS ${CMAKE_SOURCE_DIR}/.svn)

IF (NOT SVN_MARKER)
  SET (SVN_MARKER ${CMAKE_SOURCE_DIR}/CMakeLists.txt)  # Dummy file
ENDIF (NOT SVN_MARKER)
# Add a custom command to drive the svn script whenever the svn entries
# file changes.
CONFIGURE_FILE(
       ${CMAKE_CURRENT_SOURCE_DIR}/cmake_templates/svnscript.cmake.in_cmake
       ${CMAKE_CURRENT_BINARY_DIR}/svnscript.cmake
       @ONLY)

ADD_CUSTOM_COMMAND (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/marblesvnversion.h
  DEPENDS ${SVN_MARKER}
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/svnscript.cmake
)

# Add a custom target to drive the custom command.
ADD_CUSTOM_TARGET(svnversion 
       ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/marblesvnversion.h)


#############################################################
# In pedantic mode, enable warnings, halt compile on warnings
# Its good programming practice to build with no warnings....
if (PEDANTIC)
  ADD_DEFINITIONS( -Wall -Werror )
endif (PEDANTIC)

#############################################################
# Create a compiler flag so we can compile in the data path

ADD_DEFINITIONS(-DMARBLE_DATA_PATH="\\"${MARBLE_DATA_PATH}\\"")

#############################################################
# Add a compiler def so that we can conditionally compile
# code in debug mode only (e.g. extra console messages)
IF (CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DDEBUG)
ENDIF (CMAKE_BUILD_TYPE MATCHES Debug)

####################################################
# Install data
if(WIN32)
  set(data_dir data)
else(WIN32)
  if(APPLE)
    if (QTONLY)
      # for Mac OS X, everything is put inside an application bundle
      SET (CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/)
      # path for library references
      SET (CMAKE_INSTALL_NAME_DIR @executable_path/lib)
      #set info.plist properties on mac
      SET( PROGNAME ${PROJECT_NAME})
      SET( MACOSX_BUNDLE_ICON_FILE Marble.icns)
      SET( MACOSX_BUNDLE_SHORT_VERSION_STRING 0.3.0 )
      SET( MACOSX_BUNDLE_VERSION 0.3.0 )
      SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version 0.3.0 )
      #SET( CMAKE_OSX_ARCHITECTURES ppc;i386 ) #Comment out if not universal binary
      SET( CMAKE_OSX_ARCHITECTURES i386 ) #Comment out if universal binary
      #SET (lib_dir ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/lib)
      SET (data_dir   ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/resources/data)
    else (QTONLY) # KDE4 on Mac...
      set(data_dir ${DATA_INSTALL_DIR}/marble/data)
    endif (QTONLY)
  elseif(APPLE) # Linux / bsd etc...
    if (QTONLY)
      set(data_dir ${CMAKE_INSTALL_PREFIX}/share/apps/marble/data)
    else (QTONLY)
      set(data_dir ${DATA_INSTALL_DIR}/marble/data)
    endif (QTONLY)
  endif(APPLE)
endif(WIN32)

####################################################
# Descend into subdirectories
add_subdirectory(src)
add_subdirectory(data)

include(DistTarget)

####################################################
# Install extra files
# install(FILES LICENSE.txt DESTINATION ${data_dir})

############################################################
# Uninstall stuff
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_templates/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
