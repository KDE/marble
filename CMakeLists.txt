cmake_minimum_required(VERSION 2.8.12)

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
# Default to hidden visibility for symbols
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
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
include(GenerateExportHeader)
include( MarbleMacros )

####################################################
# Generate the tiles with the tilecreator at compile time
# if this option is set, srtm.jpg will not be installed but the generated tiles instead
option(MOBILE "Create a Marble version optimized for handheld devices")

####################################################
# Build a D-Bus interface for the Marble widget
# This is disabled by default for all win32, apple and Android
if(WIN32 OR APPLE OR CMAKE_SYSTEM_NAME STREQUAL Android)
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" OFF)
else()
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" ON)
endif()


#######################################################
# Specific options for building for different platforms

if(CMAKE_SYSTEM_NAME STREQUAL Android)
   add_definitions(-DANDROID)
endif()


#######################################################
# Find Qt dependencies

# Minimum Qt version
set(REQUIRED_QT_VERSION 5.3.0)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
   # TODO: still needed with ECM toolchain?
   SET(QT_QMAKE_EXECUTABLE "$ENV{Qt5_android}/bin/qmake")
endif()

find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Xml ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Network ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Test ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Script ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Widgets ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Svg ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Sql ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Concurrent ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5Quick ${REQUIRED_QT_VERSION} REQUIRED)
find_package(Qt5PrintSupport ${REQUIRED_QT_VERSION} REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
   find_package(Qt5Location ${REQUIRED_QT_VERSION} REQUIRED)
   find_package(Qt5Positioning ${REQUIRED_QT_VERSION} REQUIRED)
   find_package(Qt5Multimedia ${REQUIRED_QT_VERSION} REQUIRED)
   set ( MARBLE_NO_WEBKITWIDGETS TRUE )
else()
   find_package(Qt5WebKit ${REQUIRED_QT_VERSION})
   find_package(Qt5WebKitWidgets ${REQUIRED_QT_VERSION})

   if ( NOT Qt5WebKitWidgets_FOUND )
     set ( MARBLE_NO_WEBKITWIDGETS TRUE )
   endif()
endif()

if (BUILD_WITH_DBUS)
  find_package(Qt5DBus ${REQUIRED_QT_VERSION})
  if (NOT Qt5DBus_FOUND)
    set(MARBLE_NO_DBUS TRUE)
  endif()
else()
  set(MARBLE_NO_DBUS TRUE)
endif()

if(QTONLY)
  # Forward the old QTONLY=TRUE option to the new WITH_KDE=FALSE
  # needs to be written to cache and forcefully, otherwise macro_optional_find_package's
  # option(WITH_${_name} ON) seems to operate on a shadowed variant of the variable
  set(WITH_KF5 FALSE CACHE BOOL "Search for KF5 package" FORCE)
  # And unset it quickly. Nobody shall use it anymore anywhere.
  # to detect if KDE _should_ be used (set by user): WITH_KDE but do not use it within src/lib/
  unset(QTONLY)
endif()

SET(CMAKE_AUTOMOC TRUE)

# Use M_PI under Windows
if( WIN32 )
    add_definitions( -D_USE_MATH_DEFINES )
endif( WIN32 )

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
if (NOT QT_PLUGINS_DIR)
   set(QT_PLUGINS_DIR ${CMAKE_INSTALL_LIBDIR}/plugins)
endif()

####################################################
# Detect default for the user configurable MARBLE_DATA_PATH option

if(WIN32)
    set(data_dir data)
    set(plugin_dir plugins)
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
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
      set(plugin_dir "${CMAKE_INSTALL_PREFIX}/assets/plugins")
      set(data_dir "${CMAKE_INSTALL_PREFIX}/assets/data")
else() # Linux / bsd etc...
      set(data_dir ${CMAKE_INSTALL_FULL_DATAROOTDIR}/marble/data)
      set(plugin_dir ${CMAKE_INSTALL_FULL_LIBDIR}/marble/plugins)
endif()

if(NOT ICON_INSTALL_DIR)
	set(ICON_INSTALL_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons)
endif(NOT ICON_INSTALL_DIR)

if(NOT APPS_INSTALL_DIR)
	set(APPS_INSTALL_DIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications)
endif(NOT APPS_INSTALL_DIR)

if(NOT INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include")
endif()

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

add_definitions(
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_STRICT_ITERATORS
)

####################################################
# Options for static build
if(STATIC_BUILD)
  add_definitions(-DSTATIC_BUILD=1)
endif(STATIC_BUILD)

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
# Add global ECM & KF5 settings
set(KDE_INSTALL_DIRS_NO_DEPRECATED TRUE)
set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")


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
install(FILES LICENSE.txt DESTINATION ${MARBLE_DATA_INSTALL_PATH})

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
