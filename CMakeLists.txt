project(marble)

####################################################
# CMake Settings 
SET(CMAKE_COLOR_MAKEFILE ON)
# SET(CMAKE_SKIP_RPATH ON)

####################################################
# Generate the tiles with the tilecreator at compile time
# if this option is set, srtm.jpg will not be installed but the generated tiles instead
option(TILES_AT_COMPILETIME "Create the data for the Atlas map at compiletime" ON)
option(QTONLY "Create Marble version without KDE dependencies" OFF)
option(MOBILE "Create a Marble version optimized for handheld devices")

####################################################
# Build a D-Bus interface for marble
# This is disabled by default for all win32, apple and Qt-Only builds  
if( QTONLY OR WIN32 OR APPLE)
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" OFF)
else( QTONLY OR WIN32 OR APPLE )
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" ON)
endif( QTONLY OR WIN32 OR APPLE )

###################################################
# Check if KDE4 is available
if( NOT QTONLY AND NOT KDE4_FOUND )
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include (MacroLibrary)
endif( NOT QTONLY AND NOT KDE4_FOUND) 

#### Python support ####
set(PythonSupport_FOUND FALSE)

if( NOT QTONLY )
macro_log_feature(EXPERIMENTAL_PYTHON_BINDINGS "Experimental Python binding support for the Marble library" "Experimental Python binding support for the Marble library"
  "http://techbase.kde.org/Development/Languages/Python" FALSE ""
  "Experimental Python binding support for the Marble library. To activate it pass -DEXPERIMENTAL_PYTHON_BINDINGS=TRUE to cmake.")
endif()

if(EXPERIMENTAL_PYTHON_BINDINGS)
    macro_optional_find_package(PythonLibrary)

    macro_optional_find_package(SIP)
    if(SIP_FOUND AND SIP_VERSION STRLESS "040c02")
        message(STATUS "The version of SIP found is too old. 4.12.2 or later is needed.")
        set(SIP_FOUND)
    endif(SIP_FOUND AND SIP_VERSION STRLESS "040c02")

    include(SIPMacros)

    macro_optional_find_package(PyQt4)
    if(PYQT4_FOUND AND PYQT4_VERSION STRLESS "040804")
        message(STATUS "The version of PyQt found is too old. 4.8.4 or later is required.")
        set(PYQT4_FOUND)
    endif(PYQT4_FOUND AND PYQT4_VERSION STRLESS "040804")
    if( NOT QTONLY )
      macro_log_feature(PYQT4_FOUND "PyQt4" "PyQt4 was not found.  It is needed by marble python plugins to run. " "http://www.riverbankcomputing.co.uk/software/pyqt/intro" FALSE)
    endif()

    if (PYTHONLIBRARY_FOUND AND SIP_FOUND AND PYQT4_FOUND)
      set(PythonSupport_FOUND TRUE)
    endif (PYTHONLIBRARY_FOUND AND SIP_FOUND AND PYQT4_FOUND)

    if( NOT QTONLY )
      macro_log_feature(PythonSupport_FOUND "Python" "KDE Python support"
      "http://techbase.kde.org/Development/Languages/Python" FALSE ""
      "Needed for Python bindings to the marble widget.")
    endif()
endif(EXPERIMENTAL_PYTHON_BINDINGS)

#### End Python support ####

####################################################
# build unit tests

INCLUDE (CTest)
ENABLE_TESTING()


if( QTONLY AND NOT WIN32 )
    option( BUILD_MARBLE_TESTS "build marble tests if building QTONLY " ON )
    if( BUILD_MARBLE_TESTS )
        INCLUDE(CTest)
    endif( BUILD_MARBLE_TESTS )
else()
    # for KDE builds this gets set in the kdeedu CMakeLists.txt
    set( BUILD_MARBLE_TESTS OFF )
endif()

if( BUILD_MARBLE_TESTS )
#  SET (TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/test_data")
  #where unit test binaries should be installed to and run from
#  SET (MARBLE_TEST_DIR   ${CMAKE_CURRENT_BINARY_DIR}/tests)
endif( BUILD_MARBLE_TESTS )

####################################################
# minimum required cmake version
if( QTONLY )
    # all previous releases lack QT_QTSCRIPT_LIBRARY needed for panoramio
    # this might be replaced by a workaround
    cmake_minimum_required( VERSION 2.4.8 )

    #suppress the policy warnings while keeping the same behaviour
    if( COMMAND cmake_policy )    
    cmake_policy( SET CMP0005 OLD )
    cmake_policy( SET CMP0003 OLD )
    endif( COMMAND cmake_policy )
    
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
    set(CMAKE_MODULES_INSTALL_PATH ${CMAKE_ROOT}/Modules)
  else(QTONLY)
    set(data_dir ${DATA_INSTALL_DIR}/marble/data)
    set(plugin_dir ${PLUGIN_INSTALL_DIR}/plugins/marble)
    set(CMAKE_MODULES_INSTALL_PATH ${DATA_INSTALL_DIR}/cmake/modules)
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
      SET( MACOSX_BUNDLE_BUNDLE_NAME Marble)
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
      set(data_dir ${CMAKE_INSTALL_PREFIX}/share/marble/data)
      set(plugin_dir ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/marble/plugins)
      set(CMAKE_MODULES_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/share/marble/cmake)
      if(NOT ICON_INSTALL_DIR)
        set(ICON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/icons)
      endif(NOT ICON_INSTALL_DIR)
      if(NOT APPS_INSTALL_DIR)
        set(APPS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/applications)
      endif(NOT APPS_INSTALL_DIR)
    else (QTONLY)
      set(data_dir ${DATA_INSTALL_DIR}/marble/data)
      set(plugin_dir ${PLUGIN_INSTALL_DIR}/plugins/marble)
      set(CMAKE_MODULES_INSTALL_PATH ${DATA_INSTALL_DIR}/cmake/modules)
      set(APPS_INSTALL_DIR ${XDG_APPS_INSTALL_DIR})
    endif (QTONLY)
  endif(APPLE)
endif(WIN32)

if (NOT MARBLE_DATA_PATH)
  set (MARBLE_DATA_PATH ${data_dir})
endif (NOT MARBLE_DATA_PATH)

if (NOT MARBLE_PLUGIN_PATH)
  set (MARBLE_PLUGIN_PATH ${plugin_dir})
endif (NOT MARBLE_PLUGIN_PATH)

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
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

####################################################
# Where to look first for cmake modules, 
# before ${CMAKE_ROOT}/Modules/ is checked
set ( CMAKE_MODULE_PATH 
     ${CMAKE_SOURCE_DIR}/cmake/Modules 
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/src
     ${CMAKE_CURRENT_SOURCE_DIR}/cmake_find_rules
     ${CMAKE_CURRENT_SOURCE_DIR}/cmake_scripts
     )

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
  set( QT_USE_QTXML      ON )
  set( QT_USE_QTNETWORK  ON )
  set( QT_USE_QTTEST     ON )
  set( QT_USE_QTSCRIPT   ON )
  include( ${QT_USE_FILE} )

  # add a flag to be able to distinguish between qt 
  # and kde mode in the sources
  add_definitions(-DQTONLY)

  # Use M_PI under Windows
  if( WIN32 )
    add_definitions( -D_USE_MATH_DEFINES )
  endif( WIN32 )
endif (QTONLY)

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
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")
    # Its good programming practice to build with no warnings...
    add_definitions( -Wall -Wextra -Wundef -Wnon-virtual-dtor -Woverloaded-virtual -Wno-long-long
                     -Wchar-subscripts -Wcast-align -Wpointer-arith -Wformat-security )

    # In pedantic mode, treat warnings as errors
    if (PEDANTIC)
        add_definitions( -Werror )
    endif (PEDANTIC)
endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")

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
# reenable this if you want to debug kml tags
# add_definitions("-DDEBUG_TAGS")
####################################################
# add cmake macros
include( MarbleMacros )

####################################################
# Add the include directories

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/Projections
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/geodata
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/geodata/data
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/geodata/graphicsitem
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/geodata/handlers/dgml
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/geodata/parser
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/geodata/writer
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/geodata/scene
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/graphicsview
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/graphicsview/screengraphicsitem
  ${CMAKE_CURRENT_BINARY_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}/src/lib
)
####################################################
# Descend into subdirectories
IF( NOT QTONLY )
  add_subdirectory(doc)
ENDIF()
add_subdirectory(src)
add_subdirectory(data)

include(DistTarget)

add_subdirectory(tests)

####################################################
# Install extra files
install(FILES LICENSE.txt DESTINATION ${MARBLE_DATA_INSTALL_PATH})

####################################################
# Install CMake module
if( CMAKE_MODULES_INSTALL_PATH )
  install( FILES FindMarble.cmake
           DESTINATION ${CMAKE_MODULES_INSTALL_PATH}  )
endif( CMAKE_MODULES_INSTALL_PATH )

############################################################
# Uninstall stuff
if(QTONLY)
    # only add the uninstall target for qt-only builds
    CONFIGURE_FILE(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_templates/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    ADD_CUSTOM_TARGET(uninstall
        "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif(QTONLY)

include(MarbleCPackOptions)
include(CPack)
