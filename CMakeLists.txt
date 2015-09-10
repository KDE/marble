project(marble)

####################################################
# CMake Settings
SET(CMAKE_COLOR_MAKEFILE ON)
# SET(CMAKE_SKIP_RPATH ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Let CMake find the correct library dir instead of
# relying on the obsolete LIB_SUFFIX parameter
include(GNUInstallDirs)

# Taken from KDECompilerSettings.cmake:
# Pick sensible versions of the C and C++ standards.
# Note that MSVC does not have equivalent flags; the features are either
# supported or they are not.
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    # We use the C89 standard because that is what is common to all our
    # compilers (in particular, MSVC 2010 does not support C99)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -std=iso9899:1990")
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" AND NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

####################################################
# Where to look first for cmake modules,
# before ${CMAKE_ROOT}/Modules/ is checked
set ( CMAKE_MODULE_PATH
     ${CMAKE_SOURCE_DIR}/cmake/modules
     ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/src
     ${CMAKE_CURRENT_SOURCE_DIR}/cmake_find_rules
     ${CMAKE_CURRENT_SOURCE_DIR}/cmake_scripts
     ${CMAKE_MODULE_PATH}
     )

# add cmake macros
include( MarbleMacros )

####################################################
# Generate the tiles with the tilecreator at compile time
# if this option is set, srtm.jpg will not be installed but the generated tiles instead
option(MOBILE "Create a Marble version optimized for handheld devices")

####################################################
# Specific options for building with Qt 4 vs 5

if(ANDROID)
   if (NOT DEFINED ENV{Qt5_host})
      message(FATAL_ERROR "Configure the env variable Qt5_host to point to your host Qt5 installation.")
   else()
      message(STATUS "Qt5 host installation dir: $ENV{Qt5_host}")
   endif()

   if (NOT DEFINED ENV{Qt5_android})
      message(FATAL_ERROR "Configure the env variable Qt5_android to point to your android Qt5 installation.")
   else()
      message(STATUS "Qt5 android installation dir: $ENV{Qt5_android}")
   endif()

   if (NOT DEFINED ENV{Qt5_android_cmake})
      message(FATAL_ERROR "Configure the env variable Qt5_android_cmake to point to your qt-android-cmake installation.")
   else()
      message(STATUS "Qt5 android cmake installation dir: $ENV{Qt5_android_cmake}")
   endif()
   
   if(NOT DEFINED ENV{ANDROID_PLATFORM})
      message(FATAL_ERROR "Configure the env variable ANDROID_PLATFORM to specify your target platform (e.g.: android-18).")
   else()
      message(STATUS "Android target platform: $ENV{ANDROID_PLATFORM}")
   endif()

   # Minimum Qt version
   set(QT_REQUIRED_VERSION 5.3.0)

   SET(QT_QMAKE_EXECUTABLE "$ENV{Qt5_android}/bin/qmake")
   SET(CMAKE_PREFIX_PATH "$ENV{Qt5_android}/lib/cmake")
   
   include_directories(SYSTEM "$ENV{ANDROID_NDK}/platforms/$ENV{ANDROID_PLATFORM}/arch-arm/usr/include")
   message(STATUS "Include directories: " "$ENV{ANDROID_NDK}/platforms/$ENV{ANDROID_PLATFORM}/arch-arm/usr/include")
else()
   find_package(Qt5WebKit)
   find_package(Qt5WebKitWidgets)
   find_package(Qt5DBus)

   if ( NOT Qt5WebKit_FOUND )
     set ( MARBLE_NO_WEBKIT TRUE )
   endif()
endif()

find_package(Qt5Core)
find_package(Qt5Xml)
find_package(Qt5Network)
find_package(Qt5Test)
find_package(Qt5Script)
find_package(Qt5Widgets)
find_package(Qt5Svg)
find_package(Qt5Sql)
find_package(Qt5Concurrent)
find_package(Qt5Quick)
find_package(Qt5OpenGL)
find_package(Qt5PrintSupport)

if(QTONLY)
  # Forward the old QTONLY=TRUE option to the new WITH_KDE=FALSE
  set(WITH_KF5 FALSE)
  # And unset it quickly. Nobody shall use it anymore anywhere. Use the following variables instead:
  # to detect if building with Qt 5.x (set by us/cmake based on qmake -v): QT5BUILD
  # to detect if building with Qt 4.x (set by us/cmake based on qmake -v): QT4BUILD
  # to detect if KDE _should_ be used (set by user): WITH_KDE but do not use it within src/lib/
  unset(QTONLY)
endif()

option(QT5BUILD "Build for Qt5" ON)

if(QT5BUILD AND Qt5Core_FOUND)
  cmake_minimum_required(VERSION 2.8.12)
  set(QT4BUILD FALSE)
  set(MARBLEWIDGET marblewidget-qt5)

  include_directories( ${Qt5Svg_INCLUDE_DIRS} )
  include_directories( ${Qt5PrintSupport_INCLUDE_DIRS} )
  include_directories( ${Qt5Network_INCLUDE_DIRS} )
  include_directories( ${Qt5Script_INCLUDE_DIRS} )
  include_directories( ${Qt5Test_INCLUDE_DIRS} )
  include_directories( ${Qt5Xml_INCLUDE_DIRS} )
  include_directories( ${Qt5Sql_INCLUDE_DIRS} )
else()
  cmake_minimum_required(VERSION 2.8.6)
  unset(QT_MOC_EXECUTABLE) # somehow the qt5 check sneaks in moc for qt5 even if qmake points to qt4
  set(WITH_KF5 FALSE)
  set(QT4BUILD TRUE)
  set(MARBLEWIDGET marblewidget)
  find_package(Qt4 REQUIRED)
  set( QT_USE_QTXML         ON )
  set( QT_USE_QTNETWORK     ON )
  set( QT_USE_QTTEST        ON )
  set( QT_USE_QTSCRIPT      ON )
  set( QT_USE_QTWEBKIT      ON )
  set( QT_USE_QTSVG         ON )
  set( QT_USE_QTDECLARATIVE ON )
  set( QT_USE_QTSQL         ON )
  set( QT_USE_QTDBUS        ON )
  include( ${QT_USE_FILE} )
  marble_set_package_properties( Qt4 PROPERTIES DESCRIPTION "cross-platform application framework" )
  marble_set_package_properties( Qt4 PROPERTIES URL "http://qt.digia.com/" )
  marble_set_package_properties( Qt4 PROPERTIES TYPE REQUIRED PURPOSE "core framework" )
  IF ( NOT QT_QTDECLARATIVE_FOUND )
    # older cmake versions have a FindQt4.cmake without support for declarative,
    # but the library may still be available
    FIND_PACKAGE(QtDeclarative)
    include_directories(${QT_QTDECLARATIVE_INCLUDE_DIR})
  endif()
  if ( NOT QT_QTWEBKIT_FOUND )
    set ( MARBLE_NO_WEBKIT TRUE )
  endif()
endif()

SET(CMAKE_AUTOMOC TRUE)

# Use M_PI under Windows
if( WIN32 )
    add_definitions( -D_USE_MATH_DEFINES )
endif( WIN32 )

####################################################
# Build a D-Bus interface for marble
# This is disabled by default for all win32, apple and Qt-Only builds
if(WIN32 OR APPLE OR ANDROID)
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" OFF)
else()
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" ON)
endif()

####################################################
# build unit tests

INCLUDE (CTest)
ENABLE_TESTING()

option( BUILD_MARBLE_TESTS "Build unit tests" ON )
add_feature_info("Unit tests" BUILD_MARBLE_TESTS "Build unit tests. Toggle with BUILD_MARBLE_TESTS=YES/NO. 'make test' will run all.")

if( BUILD_MARBLE_TESTS )
#  SET (TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/test_data")
  #where unit test binaries should be installed to and run from
#  SET (MARBLE_TEST_DIR   ${CMAKE_CURRENT_BINARY_DIR}/tests)
endif( BUILD_MARBLE_TESTS )

####################################################
set (PEDANTIC FALSE CACHE BOOL "Determines if we should compile with -Wall -Werror.")
set (ENABLE_TESTS FALSE CACHE BOOL "Build unit tests?")
set (WITH_DESIGNER_PLUGIN TRUE CACHE BOOL "Build plugins for Qt Designer")
add_feature_info("Qt Designer plugins" WITH_DESIGNER_PLUGIN "Marble widget support in Qt Designer. Toggle with WITH_DESIGNER_PLUGIN=YES/NO")

set(EXEC_INSTALL_PREFIX  ${CMAKE_INSTALL_PREFIX} CACHE PATH  "Base directory for executables and libraries" FORCE)
if (QT5BUILD)
   set(QT_PLUGINS_DIR ${CMAKE_INSTALL_LIBDIR}/plugins)
endif()

####################################################
# Detect default for the user configurable MARBLE_DATA_PATH option

if(WIN32)
    set(data_dir data)
    set(plugin_dir plugins)
    set(CMAKE_MODULES_INSTALL_PATH ${CMAKE_ROOT}/Modules)
elseif(APPLE)
    # needed for finding bundle path in e.g. katlasdir.h
    FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices )
    MARK_AS_ADVANCED (APP_SERVICES_LIBRARY)
    SET(MAC_EXTRA_LIBS ${APP_SERVICES_LIBRARY})
      # for Mac OS X, everything is put inside an application bundle
      SET (CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/)
      # path for library references
      SET (CMAKE_INSTALL_NAME_DIR @executable_path/lib)
      # install the Info.plist file
      install(FILES src/mac/Info.plist DESTINATION ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents)
      #SET (lib_dir ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/lib)
      SET (data_dir   ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/resources/data)
      SET (plugin_dir   ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/resources/plugins)
else() # Linux / bsd etc...
      set(data_dir ${CMAKE_INSTALL_PREFIX}/share/marble/data)
      set(plugin_dir ${CMAKE_INSTALL_FULL_LIBDIR}/marble/plugins)
      set(CMAKE_MODULES_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/share/marble/cmake)
endif()

if(NOT ICON_INSTALL_DIR)
  set(ICON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/icons)
endif(NOT ICON_INSTALL_DIR)

if(NOT APPS_INSTALL_DIR)
  set(APPS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/applications)
endif(NOT APPS_INSTALL_DIR)

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

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

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
####################################################

####################################################
# Add the include directories

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble/projections
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble/geodata
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble/geodata/data
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble/geodata/graphicsitem
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble/geodata/handlers/dgml
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble/geodata/parser
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble/geodata/writer
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble/geodata/scene
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/marble/graphicsview
  ${CMAKE_CURRENT_BINARY_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}/src/lib/marble
)
####################################################
# Descend into subdirectories
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(data)

include(DistTarget)

add_subdirectory(tests)

option(BUILD_MARBLE_TOOLS "Build various tools related to Marble" OFF)
add_feature_info("Marble tools" BUILD_MARBLE_TOOLS "Build various Marble tools for e.g. file format conversion. Toggle with BUILD_MARBLE_TOOLS=YES/NO.")
if(BUILD_MARBLE_TOOLS)
  add_subdirectory(tools)
endif()

option(BUILD_MARBLE_EXAMPLES "Build C++ examples showing how to use the Marble library" OFF)
add_feature_info("Marble library C++ examples" BUILD_MARBLE_EXAMPLES "Build C++ examples showing how to use the Marble library. Toggle with BUILD_MARBLE_EXAMPLES=YES/NO.")
if(BUILD_MARBLE_EXAMPLES)
  add_subdirectory(examples/cpp)
endif()

####################################################
# Install extra files
install(FILES marble.appdata.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/appdata/)

####################################################
# Install CMake module

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/FindMarble.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/FindMarble.cmake
)

if( CMAKE_MODULES_INSTALL_PATH AND NOT WIN32 )
  install( FILES ${CMAKE_CURRENT_BINARY_DIR}/FindMarble.cmake
           DESTINATION ${CMAKE_MODULES_INSTALL_PATH}  )
endif()

############################################################
# Uninstall stuff
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

include(MarbleCPackOptions)

marble_feature_summary(WHAT ALL)
