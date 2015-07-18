# before add_subdirectory() !
if(MSVC)
  add_definitions(-D_USE_MATH_DEFINES=1)
endif(MSVC)

if(BUILD_WITH_DBUS)
add_definitions(-DMARBLE_DBUS)
endif(BUILD_WITH_DBUS)

if( QT_QTDECLARATIVE_FOUND OR Qt5Quick_FOUND )
  OPTION(IGNORE_CMAKE_INSTALL_PREFIX_FOR_DECLARATIVE_PLUGINS "Install Marble Qt Declarative plugins to QT_IMPORTS_DIR even if that is out of the CMAKE_INSTALL_PREFIX")
  IF(DEFINED QT_IMPORTS_DIR)
    SET(MARBLE_QT_IMPORTS_DIR ${QT_IMPORTS_DIR})
  ELSE()
    # Fallback when FindQt4.cmake (part of cmake) does not define QT_IMPORTS_DIR. Get it from qmake instead.
    IF( QT4BUILD )
      EXEC_PROGRAM( ${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_IMPORTS" OUTPUT_VARIABLE MARBLE_QT_IMPORTS_DIR )
    ELSE()
      EXEC_PROGRAM( qmake ARGS "-query QT_INSTALL_QML" OUTPUT_VARIABLE MARBLE_QT_IMPORTS_DIR )
    ENDIF()
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

add_subdirectory( lib/marble )
add_subdirectory( lib/astro )

#FIXME: Port to Android
if(NOT ANDROID)
add_subdirectory( plugins )
endif()

option(BUILD_MARBLE_APPS "Build Marble Desktop/Mobile applications" ON)
add_feature_info("Marble Desktop/Mobile applications" BUILD_MARBLE_APPS "Build Marble Desktop/Mobile applications. Toggle with BUILD_MARBLE_APPS=YES/NO.")
if(BUILD_MARBLE_APPS)
  add_subdirectory( apps )
endif()

add_subdirectory( bindings )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-marble.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-marble.h
)

if(APPLE)
  add_subdirectory( mac )
endif(APPLE)

# FIXME ktimezonewidget.h is gone
#add_subdirectory( plasmoid )

add_subdirectory( plasmarunner )
add_subdirectory( thumbnailer )
