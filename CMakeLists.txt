cmake_minimum_required(VERSION 3.10)

# Set minimum OS X target
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "21")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "90")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(marble VERSION ${RELEASE_SERVICE_VERSION})

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

# RPATH/RUNPATH settings
if (UNIX)
    # Add CMAKE_INSTALL_FULL_LIBDIR to the RPATH to be used when installing,
    # but only if it isn't a standard system directory.
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_FULL_LIBDIR}" isSystemLibDir)
    list(FIND CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES      "${CMAKE_INSTALL_FULL_LIBDIR}" isSystemCxxLibDir)
    list(FIND CMAKE_C_IMPLICIT_LINK_DIRECTORIES        "${CMAKE_INSTALL_FULL_LIBDIR}" isSystemCLibDir)
    if("${isSystemLibDir}" STREQUAL "-1"  AND  "${isSystemCxxLibDir}" STREQUAL "-1"  AND  "${isSystemCLibDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
    endif()

    # Add directories which are in the linker search path (but outside the project)
    # to the RPATH to be used when installing
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    # no libs or plugins are designed to be used from the build dir, so directly link with install rpath
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (UNIX)

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
# Provide opt-in switch for mobile variant, which is not only useful for Android.
option(BUILD_TOUCH "Build touch-friendly Marble version (on Android this option is bypassed)" OFF)

####################################################
# Build a D-Bus interface for the Marble widget
# This is disabled by default for all win32, apple and Android
if(WIN32 OR APPLE OR ANDROID)
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" OFF)
else()
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" ON)
endif()


#######################################################
# Specific options for building for different platforms

if(ANDROID)
   add_definitions(-DANDROID)
endif()


#######################################################
# Find Qt dependencies

find_package(Protobuf)
set_package_properties(Protobuf PROPERTIES TYPE OPTIONAL PURPOSE "Parsing of OSM PBF files.")

if(ANDROID)
    set(REQUIRED_QT_VERSION 5.7.0)
   # TODO: still needed with ECM toolchain?
   SET(QT_QMAKE_EXECUTABLE "$ENV{Qt5_android}/bin/qmake")
else()
    set(REQUIRED_QT_VERSION 5.7.0)
endif()

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED
    COMPONENTS
        Core
        Xml
        Network
        Test
        Widgets
        Svg
        Sql
        Concurrent
        Quick
        PrintSupport
)

if(ANDROID)
    find_package(Qt5Positioning ${REQUIRED_QT_VERSION} REQUIRED)
    find_package(Qt5Multimedia ${REQUIRED_QT_VERSION} REQUIRED)

   set ( MARBLE_NO_WEBKITWIDGETS TRUE )
else()
    find_package(Qt5WebChannel ${REQUIRED_QT_VERSION})
    find_package(Qt5WebEngine ${REQUIRED_QT_VERSION})
    find_package(Qt5WebEngineWidgets ${REQUIRED_QT_VERSION})

   if ( NOT Qt5WebEngineWidgets_FOUND )
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
    set(locale_dir "${data_dir}/locale")
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
      set(locale_dir "${data_dir}/locale")
      SET (plugin_dir   ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/resources/plugins)
elseif(ANDROID)
      set(plugin_dir "${CMAKE_INSTALL_PREFIX}/assets/plugins")
      set(data_dir "${CMAKE_INSTALL_PREFIX}/assets/data")
      set(locale_dir "${data_dir}/locale")
else() # Linux / bsd etc...
      set(data_dir ${CMAKE_INSTALL_FULL_DATAROOTDIR}/marble/data)
      set(locale_dir "${CMAKE_INSTALL_FULL_DATAROOTDIR}/locale")
      set(plugin_dir ${CMAKE_INSTALL_FULL_LIBDIR}/marble/plugins)
endif()

if(NOT ICON_INSTALL_DIR)
	set(ICON_INSTALL_DIR share/icons)
endif(NOT ICON_INSTALL_DIR)

if(NOT APPS_INSTALL_DIR)
	set(APPS_INSTALL_DIR share/applications)
endif(NOT APPS_INSTALL_DIR)

if(NOT INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR "include")
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
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050700
)

if (NOT WIN32)
    # Strict iterators can't be used on Windows, they lead to a link error
    # when application code iterates over a QVector<QPoint> for instance
    # See example at https://bugreports.qt.io/browse/QTBUG-78112
    add_definitions(-DQT_STRICT_ITERATORS)
endif()
####################################################
# Options for static build
if(STATIC_BUILD)
  set(BUILD_SHARED_LIBS FALSE)
  add_definitions(-DSTATIC_BUILD=1)
else()
  set(BUILD_SHARED_LIBS TRUE)
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
set(REQUIRED_ECM_VERSION 1.7.0)
set(KDE_INSTALL_DIRS_NO_DEPRECATED TRUE)
set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
set(REQUIRED_KF5_MIN_VERSION 5.7.0)

find_package(ECM ${REQUIRED_ECM_VERSION} QUIET)
if(ECM_FOUND)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

  include(KDEInstallDirs)
  # KDECMakeSettings expects LIB_INSTALL_DIR set
  set(LIB_INSTALL_DIR ${KDE_INSTALL_LIBDIR})
  include(KDECMakeSettings)
  include(ECMInstallIcons)
  include(ECMEnableSanitizers)
  include(ECMCoverageOption)
endif()

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
# Handle translation catalogs of KDE Application release tarball of marble
# (for custom bundle translation handling see data/lang/README)

# KDE Application release tarballs have the respective po files added on tarball creation,
# which are otherwise stored and edited separately on the KDE svn servers.
# Toplevel subdir "poqm/" holds all the po files which should be processed and installed as qm files
marble_install_po_files_as_qm(poqm)

# Toplevel subdir "po/" holds all the po files which should be processed and installed as mo files
# Those are used by app and plugin code building against KI18n.
# So the respective macro ki18n_install can be used.
# Likewise, it contains the translated user documentation, which is handled by kdoctools_install.
# Just, the macros are not available here in the toplevel CMakeLists.txt file.
# As a temporary hack (your task to improve it, dear reader :) ) the macros
# are called only from src/apps/marble-kde/CMakeLists.txt.
# That should catch 99.9 % cases of builds from released tarball, where all the
# KF5-dependent apps and plugins will be built and installed together.
# Currently these are:
# src/apps/marble-kde
# src/plasmarunner
# src/plasma/applets/worldclock
# src/plasma/wallpapers/worldmap
#
# Let releaseme know about this:
# SKIP_PO_INSTALL


####################################################
# Install extra files
install(FILES LICENSES/LGPL-2.1-or-later.txt
        DESTINATION ${MARBLE_DATA_INSTALL_PATH}
        RENAME LICENSE.txt)

############################################################
# Uninstall stuff
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

include(MarbleCPackOptions)

marble_feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES)
