# before add_subdirectory() !
if(MSVC)
  add_definitions(-D_USE_MATH_DEFINES=1)
endif(MSVC)

if(NOT MARBLE_NO_DBUS)
  add_definitions(-DMARBLE_DBUS)
endif()

# disabled for now:
# no public Marble QML plugins are installed, libmarbledeclarative is only a shared lib
if (FALSE)
if( Qt6Quick_FOUND )
  OPTION(IGNORE_CMAKE_INSTALL_PREFIX_FOR_DECLARATIVE_PLUGINS "Install Marble Qt Declarative plugins to QT_IMPORTS_DIR even if that is out of the CMAKE_INSTALL_PREFIX")
  IF(QT_IMPORTS_DIR)
    SET(MARBLE_QT_IMPORTS_DIR ${QT_IMPORTS_DIR})
  ELSE()
    # Fallback when FindQt.cmake (part of cmake) does not define QT_IMPORTS_DIR. Get it from qmake instead.
    EXEC_PROGRAM( qmake ARGS "-query QT_INSTALL_QML" OUTPUT_VARIABLE MARBLE_QT_IMPORTS_DIR )
  ENDIF()
  STRING(LENGTH "${CMAKE_INSTALL_PREFIX}" _MARBLE_CMAKE_INSTALL_PREFIX_LEN)
  STRING(LENGTH "${MARBLE_QT_IMPORTS_DIR}" _MARBLE_QT_IMPORTS_DIR_LEN)
  IF(NOT ${CMAKE_INSTALL_PREFIX} STREQUAL /usr/local)
    IF(NOT ${_MARBLE_QT_IMPORTS_DIR_LEN} LESS ${_MARBLE_CMAKE_INSTALL_PREFIX_LEN})
      STRING(SUBSTRING ${MARBLE_QT_IMPORTS_DIR} 0 ${_MARBLE_CMAKE_INSTALL_PREFIX_LEN} _MARBLE_QT_IMPORTS_DIR_START)
      IF(NOT ${CMAKE_INSTALL_PREFIX} STREQUAL ${_MARBLE_QT_IMPORTS_DIR_START})
        IF(NOT ${IGNORE_CMAKE_INSTALL_PREFIX_FOR_DECLARATIVE_PLUGINS})
          MESSAGE(STATUS "Please include ${MARBLE_PLUGIN_INSTALL_PATH} in the QML_IMPORT_PATH environment variable to use Marble's Qt declarative plugins.")
          MESSAGE(STATUS "Note: Marble Desktop does not make use of declarative plugins. Ignore the message above if you only intend to use the Desktop version.")
          SET(MARBLE_QT_IMPORTS_DIR ${MARBLE_PLUGIN_INSTALL_PATH})
        ENDIF()
      ENDIF()
    ELSE()
        IF(NOT ${IGNORE_CMAKE_INSTALL_PREFIX_FOR_DECLARATIVE_PLUGINS})
          MESSAGE(STATUS "Please include ${MARBLE_PLUGIN_INSTALL_PATH} in the QML_IMPORT_PATH environment variable to use Marble's Qt declarative plugins.")
          MESSAGE(STATUS "Note: Marble Desktop does not make use of declarative plugins. Ignore the message above if you only intend to use the Desktop version.")
          SET(MARBLE_QT_IMPORTS_DIR ${MARBLE_PLUGIN_INSTALL_PATH})
        ENDIF()
    ENDIF()
  ENDIF()
ENDIF()
endif()

macro_optional_find_package(ZLIB QUIET)
marble_set_package_properties(ZLIB PROPERTIES DESCRIPTION "general-purpose lossless data-compression library")
marble_set_package_properties(ZLIB PROPERTIES URL "https://www.zlib.net/")
marble_set_package_properties(ZLIB PROPERTIES TYPE OPTIONAL PURPOSE "When found, system zlib is used. When not found, a Marble internal zlib is used.")
if(NOT ZLIB_FOUND)
  add_subdirectory( 3rdparty/zlib )
  set(ZLIB_LIBRARIES zlibstatic)
  set(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib" "${CMAKE_CURRENT_BINARY_DIR}/3rdparty/zlib")
endif()

add_subdirectory( 3rdparty/o5mreader )
add_subdirectory( 3rdparty/sgp4 )
add_subdirectory( lib/marble )
add_subdirectory( lib/astro )
add_subdirectory( lib/marble/declarative )

add_subdirectory( plugins )

option(BUILD_MARBLE_APPS "Build Marble Desktop/Mobile applications" ON)
add_feature_info("Marble Desktop/Mobile applications" BUILD_MARBLE_APPS "Build Marble Desktop/Mobile applications. Toggle with BUILD_MARBLE_APPS=YES/NO.")
if(BUILD_MARBLE_APPS)
  add_subdirectory( apps )
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-marble.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-marble.h
)

if(APPLE)
  add_subdirectory( mac )
endif(APPLE)

# Default logging for where no specific category is set
ecm_qt_export_logging_category(
    IDENTIFIER "MARBLE_DEFAULT"
    CATEGORY_NAME "marble_default"
    EXPORT marble
    DESCRIPTION "Marble (default)"
)

ecm_qt_install_logging_categories(
    EXPORT marble
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)

add_subdirectory(plasma)
add_subdirectory(plasmarunner)
add_subdirectory(thumbnailer)
