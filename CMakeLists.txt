project(marble)

####################################################
# CMake Settings 
SET(CMAKE_COLOR_MAKEFILE ON)

####################################################
# Generate the tiles with the tilecreator at compile time
# if this option is set, srtm.jpg will not be installed but the generated tiles instead
option(TILES_AT_COMPILETIME "generate the srtm tiles at compiletime" ON)

####################################################
# build unit tests
if( QTONLY )
    option( BUILD_MARBLE_TESTS "build marble tests if building QTONLY " OFF )
    if( BUILD_MARBLE_TESTS )
        enable_testing()
    endif( BUILD_MARBLE_TESTS )
else( QTONLY )
    # for KDE builds this gets set in the kdeedu CMakeLists.txt
    set( BUILD_MARBLE_TESTS ON )
endif( QTONLY )

####################################################
# User configurable options
if(KDE4_FOUND)
  set (QTONLY FALSE CACHE BOOL "Determines if we should compile for Qt only.")
else(KDE4_FOUND)
  set (QTONLY TRUE CACHE BOOL "Determines if we should compile for Qt only.")
endif(KDE4_FOUND)

set (PEDANTIC FALSE CACHE BOOL "Determines if we should compile with -Wall -Werror.")
set (ENABLE_TESTS FALSE CACHE BOOL "Build unit tests?")
set (WITH_DESIGNER_PLUGIN FALSE CACHE BOOL "Build designer plugin?")
if (QTONLY)
    set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
    set(EXEC_INSTALL_PREFIX  ${CMAKE_INSTALL_PREFIX} CACHE PATH  "Base directory for executables and libraries" FORCE)
endif (QTONLY)
####################################################
# Detect default for the user configurable MARBLE_DATA_PATH option

if(WIN32)
  if(QTONLY)
    set(data_dir data)
    set(plugin_dir plugins)
  else(QTONLY)
    set(data_dir ${DATA_INSTALL_DIR}/marble/data)
    set(plugin_dir ${PLUGIN_INSTALL_DIR}/plugins/marble)
  endif(QTONLY)
else(WIN32)
  if(APPLE)
    # needed for finding bundle path in e.g. katlasdir.h
    FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices )
    MARK_AS_ADVANCED (APP_SERVICES_LIBRARY)
    SET(MAC_EXTRA_LIBS ${APP_SERVICES_LIBRARY})
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
      SET (plugin_dir   ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/resources/plugins)
    else (QTONLY) # KDE4 on Mac...
      set(data_dir ${DATA_INSTALL_DIR}/marble/data)
      set(plugin_dir ${PLUGIN_INSTALL_DIR}/plugins/marble)
    endif (QTONLY)
  else(APPLE) # Linux / bsd etc...
    if (QTONLY)
      set(data_dir ${CMAKE_INSTALL_PREFIX}/share/apps/marble/data)
      set(plugin_dir ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/marble/plugins)
    else (QTONLY)
      set(data_dir ${DATA_INSTALL_DIR}/marble/data)
      set(plugin_dir ${PLUGIN_INSTALL_DIR}/plugins/marble)
    endif (QTONLY)
  endif(APPLE)
endif(WIN32)

if (NOT MARBLE_DATA_PATH)
  set (MARBLE_DATA_PATH ${data_dir})
endif (NOT MARBLE_DATA_PATH)

if (NOT MARBLE_PLUGIN_PATH)
  set (MARBLE_PLUGIN_PATH ${plugin_dir})
endif (NOT MARBLE_PLUGIN_PATH)

#MESSAGE( STATUS, "MARBLE_PLUGIN_PATH: ${MARBLE_PLUGIN_PATH}" )

if (NOT MARBLE_DATA_INSTALL_PATH)
  set (MARBLE_DATA_INSTALL_PATH ${MARBLE_DATA_PATH})
endif (NOT MARBLE_DATA_INSTALL_PATH)

if (NOT MARBLE_PLUGIN_INSTALL_PATH)
  set (MARBLE_PLUGIN_INSTALL_PATH ${MARBLE_PLUGIN_PATH})
endif (NOT MARBLE_PLUGIN_INSTALL_PATH)

#MESSAGE( STATUS, "MARBLE_PLUGIN_INSTALL_PATH: ${MARBLE_PLUGIN_INSTALL_PATH}" )

if(WIN32)
  set (STATIC_BUILD FALSE CACHE BOOL "Link to static Qt libs (win32 only)?")
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
     ${CMAKE_CURRENT_SOURCE_DIR}/cmake_scripts
     )

####################################################
# Enable ctest
IF (ENABLE_TESTS)
  ENABLE_TESTING()
  SET (TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/test_data")
  #where unit test binaries should be installed to and run from
  SET (MARBLE_TEST_DIR   ${CMAKE_CURRENT_BINARY_DIR}/tests)
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
  set( QT_USE_QTXML      TRUE )
  set( QT_USE_QTNETWORK  TRUE )
  set( QT_USE_QTTEST     TRUE )
  set( QT_USE_QTSCRIPT   TRUE )
  include( ${QT_USE_FILE} )
  
  # add a flag to be able to distinguish between qt 
  # and kde mode in the sources
  add_definitions(-DQTONLY)
  message("+++++++++${QT_QTSCRIPT_LIBRARY}+++++++++++++")
else(QTONLY)
  # search packages used by KDE
  find_package(KDE4 REQUIRED)
  include (KDE4Defaults)
  include (MacroLibrary)
endif (QTONLY)
message( "${QT_QTSCRIPT_LIBRARY}" )
#############################################################
# Create marblesvnversion.h
# see http://public.kitware.com/pipermail/cmake/2006-July/010299.html
# We only do this if we are in a .svn dir

FIND_FILE(SVN_MARKER entries PATHS ${CMAKE_SOURCE_DIR}/.svn)

IF (NOT EXISTS SVN_MARKER)
  SET (SVN_MARKER ${CMAKE_SOURCE_DIR}/CMakeLists.txt)  # Dummy file
ENDIF (NOT EXISTS SVN_MARKER)
# Add a custom command to drive the svn script whenever the svn entries
# file changes.
CONFIGURE_FILE(
       "${CMAKE_CURRENT_SOURCE_DIR}/cmake_templates/svnscript.cmake.in_cmake"
       "${CMAKE_CURRENT_BINARY_DIR}/svnscript.cmake"
       @ONLY)

ADD_CUSTOM_COMMAND (
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/marblesvnversion.h"
  DEPENDS ${SVN_MARKER}
  COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/svnscript.cmake"
)

# Add a custom target to drive the custom command.
ADD_CUSTOM_TARGET(svnversion 
       ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/marblesvnversion.h")


#############################################################
# In pedantic mode, enable warnings, halt compile on warnings
# Its good programming practice to build with no warnings....
if (PEDANTIC)
  ADD_DEFINITIONS( -Wall -Werror )
endif (PEDANTIC)

#############################################################
# Add a compiler def so that we can conditionally compile
# code in debug mode only (e.g. extra console messages)
IF (CMAKE_BUILD_TYPE MATCHES Debug)
    IF(NOT MINGW)
        ADD_DEFINITIONS(-DDEBUG)
    ELSE(NOT MINGW)
        REMOVE_DEFINITIONS( -DQT_NO_DEBUG )
    ENDIF(NOT MINGW)
ENDIF (CMAKE_BUILD_TYPE MATCHES Debug)

####################################################
# on Win32 set the debug postfix
if(WIN32)
  # distinguish between debug and release plugin
  SET(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

####################################################
# add cmake macros
include( MarbleMacros )

####################################################
# Descend into subdirectories
add_subdirectory(src)
add_subdirectory(data)

include(DistTarget)

IF (ENABLE_TESTS)
  add_subdirectory(tests)
ENDIF (ENABLE_TESTS)

####################################################
# Install extra files
install(FILES LICENSE.txt DESTINATION ${MARBLE_DATA_INSTALL_PATH})

############################################################
# Uninstall stuff
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_templates/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
