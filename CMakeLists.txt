cmake_minimum_required(VERSION 3.20)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "12")
set(RELEASE_SERVICE_VERSION_MICRO "2")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

set(MARBLE_LIB_SOVERSION 28)

project(marble VERSION ${RELEASE_SERVICE_VERSION})

set(KF_MIN_VERSION "6.5.0")
set(QT_MIN_VERSION "6.6")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH
    ${ECM_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_find_rules
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_scripts
    ${CMAKE_MODULE_PATH}
)

include(FeatureSummary)
include(GNUInstallDirs)
include(GenerateExportHeader)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

include(ECMSetupVersion)
include(ECMAddTests)
include(ECMInstallIcons)
include(ECMEnableSanitizers)
include(ECMCoverageOption)
include(ECMQtDeclareLoggingCategory)
include(ECMQmlModule)
include(ECMDeprecationSettings)
include(MarbleMacros)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX MARBLE
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/src/lib/marble/marble_version.h
    SOVERSION 28
)

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_KEYWORDS)

####################################################
# Generate the tiles with the tilecreator at compile time
# if this option is set, srtm.jpg will not be installed but the generated tiles instead
option(MOBILE "Create a Marble version optimized for handheld devices" OFF)

####################################################
# Provide opt-in switch for mobile variant, which is not only useful for Android.
option(BUILD_TOUCH "Build touch-friendly Marble version (on Android this option is bypassed)" OFF)

option(BUILD_QT_AND_KDE "Build pure Qt version and KDE version. Only useful for distributions who split the marble in two packages. Otherwise just provide the KDE version to your users.")

####################################################
# Build a D-Bus interface for the Marble widget
# This is disabled by default for all win32, apple and Android
if(WIN32 OR APPLE OR ANDROID)
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" OFF)
else()
    option(BUILD_WITH_DBUS "Build the D-Bus interface for the Marble widget" ON)
endif()

option(BUILD_MARBLE_APPS "Build the main Marble applications" ON)
add_feature_info(MarbleApps BUILD_MARBLE_APPS "Build the main Marble applications")
add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)
ecm_set_disabled_deprecation_versions(QT 5.15
    KF 6.7.0
)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED
    COMPONENTS
        Core
        Xml
        Network
        Widgets
        Svg
        SvgWidgets
        Sql
        Concurrent
        Quick
        PrintSupport
        Core5Compat
)

if(BUILD_TESTING)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Test)
endif()

find_package(KF6 ${KF_MIN_VERSION} COMPONENTS CoreAddons I18n Config)
if(NOT ANDROID)
    find_package(KF6 ${KF_MIN_VERSION} COMPONENTS Crash Parts)
endif()

set_package_properties(KF6 PROPERTIES
    PURPOSE "Required for building the applications"
    TYPE RECOMMENDED
)

find_package(Protobuf)
set_package_properties(Protobuf PROPERTIES TYPE OPTIONAL PURPOSE "Parsing of OSM PBF files.")

if(ANDROID)
    find_package(Qt6Positioning ${REQUIRED_QT_VERSION} REQUIRED)
    find_package(Qt6Multimedia ${REQUIRED_QT_VERSION} REQUIRED)

   set(MARBLE_NO_WEBKITWIDGETS TRUE)
else()
    find_package(Qt6WebChannel ${REQUIRED_QT_VERSION})
    find_package(Qt6WebEngineWidgets ${REQUIRED_QT_VERSION})
    find_package(Qt6WebEngineQuick ${REQUIRED_QT_VERSION})

   if( NOT Qt6WebEngineWidgets_FOUND)
     set(MARBLE_NO_WEBKITWIDGETS TRUE)
   endif()
endif()

if(BUILD_WITH_DBUS)
  find_package(Qt6DBus ${REQUIRED_QT_VERSION})
  if(NOT Qt6DBus_FOUND)
    set(MARBLE_NO_DBUS TRUE)
  endif()
else()
  set(MARBLE_NO_DBUS TRUE)
endif()

SET(CMAKE_AUTOMOC TRUE)

# Use M_PI under Windows
if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif( WIN32)

####################################################
# build unit tests

INCLUDE (CTest)
ENABLE_TESTING()

####################################################
set(PEDANTIC FALSE CACHE BOOL "Determines if we should compile with -Wall -Werror.")
set(WITH_DESIGNER_PLUGIN TRUE CACHE BOOL "Build plugins for Qt Designer")
add_feature_info("Qt Designer plugins" WITH_DESIGNER_PLUGIN "Marble widget support in Qt Designer. Toggle with WITH_DESIGNER_PLUGIN=YES/NO")

set(EXEC_INSTALL_PREFIX  ${CMAKE_INSTALL_PREFIX} CACHE PATH  "Base directory for executables and libraries" FORCE)
if(NOT QT_PLUGINS_DIR)
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
    FIND_LIBRARY(APP_SERVICES_LIBRARY ApplicationServices)
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

if(NOT MARBLE_DATA_PATH)
  set(MARBLE_DATA_PATH ${data_dir})
endif(NOT MARBLE_DATA_PATH)

if(NOT MARBLE_PLUGIN_PATH)
  set(MARBLE_PLUGIN_PATH ${plugin_dir})
endif(NOT MARBLE_PLUGIN_PATH)

if(NOT MARBLE_DATA_INSTALL_PATH)
  set(MARBLE_DATA_INSTALL_PATH ${MARBLE_DATA_PATH})
endif(NOT MARBLE_DATA_INSTALL_PATH)

if(NOT MARBLE_PLUGIN_INSTALL_PATH)
  set(MARBLE_PLUGIN_INSTALL_PATH ${MARBLE_PLUGIN_PATH})
endif(NOT MARBLE_PLUGIN_INSTALL_PATH)

#message( STATUS, "MARBLE_PLUGIN_INSTALL_PATH: ${MARBLE_PLUGIN_INSTALL_PATH}")

if(WIN32)
  set(STATIC_BUILD FALSE CACHE BOOL "Link to static Qt libs (win32 only)?")
endif(WIN32)

add_definitions(
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
)

if(NOT WIN32)
    # Strict iterators can't be used on Windows, they lead to a link error
    # when application code iterates over a QList<QPoint> for instance
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
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")
    # Its good programming practice to build with no warnings...
    add_definitions( -Wall -Wextra -Wundef -Wnon-virtual-dtor -Woverloaded-virtual -Wno-long-long
                     -Wchar-subscripts -Wcast-align -Wpointer-arith -Wformat-security)

    # In pedantic mode, treat warnings as errors
    if(PEDANTIC)
        add_definitions( -Werror)
    endif(PEDANTIC)
endif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_C_COMPILER MATCHES "icc")

#############################################################
# Add a compiler def so that we can conditionally compile
# code in debug mode only (e.g. extra console messages)
IF (CMAKE_BUILD_TYPE MATCHES Debug)
    IF(NOT MINGW)
        ADD_DEFINITIONS(-DDEBUG)
    ELSE(NOT MINGW)
        REMOVE_DEFINITIONS( -DQT_NO_DEBUG)
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
# Add global ECM & KF6 settings
set(KDE_INSTALL_DIRS_NO_DEPRECATED TRUE)
set(KDE_SKIP_UNINSTALL_TARGET ON CACHE BOOL "KDE uninstall target must be disabled")
set(REQUIRED_KF6_MIN_VERSION 6.4.0)

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

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

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
# As a temporary hack (your task to improve it, dear reader :)) the macros
# are called only from src/apps/marble-kde/CMakeLists.txt.
# That should catch 99.9 % cases of builds from released tarball, where all the
# KF6-dependent apps and plugins will be built and installed together.
# Currently these are:
# src/apps/marble-kde
# src/plasmarunner
# src/plasma/applets/worldclock
# src/plasma/wallpapers/worldmap
#
# Let releaseme know about this:
# SKIP_PO_INSTALL

include(MarbleCPackOptions)

marble_feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES)


file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h tests/*.h tests/*.cpp examples/*.h examples/*.cpp)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
