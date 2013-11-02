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
    IF( QT4_FOUND )
      EXEC_PROGRAM( ${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_IMPORTS" OUTPUT_VARIABLE MARBLE_QT_IMPORTS_DIR )
    ELSE()
      EXEC_PROGRAM( qmake ARGS "-query QT_INSTALL_QML" OUTPUT_VARIABLE MARBLE_QT_IMPORTS_DIR )
    ENDIF()
  ENDIF()
  STRING(LENGTH ${CMAKE_INSTALL_PREFIX} _MARBLE_CMAKE_INSTALL_PREFIX_LEN)
  STRING(LENGTH ${MARBLE_QT_IMPORTS_DIR} _MARBLE_QT_IMPORTS_DIR_LEN)
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

add_subdirectory( apps )
add_subdirectory( lib/marble )
add_subdirectory( plugins )
add_subdirectory( bindings )

if( NOT QTONLY AND KDE4_PLASMA_LIBS )   # don't try to build this against kdelibs < 4.2
   add_subdirectory( plasmoid )
   add_subdirectory( plasmarunner )
endif( NOT QTONLY AND KDE4_PLASMA_LIBS )

if(APPLE)
  add_subdirectory( mac )
endif(APPLE)
# FIXME: The original QT4_ADD_RESOURCES should be extended to support this filetype too
#
# QT4_ADD_RESOURCE2(outfiles inputfile ... )
# TODO  perhaps consider adding support for compression and root options to rcc

if (MINGW)
MACRO (QT4_ADD_RESOURCES2 outfiles )

FOREACH (it ${ARGN})
  GET_FILENAME_COMPONENT(outfilename ${it} NAME_WE)
  GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
  GET_FILENAME_COMPONENT(rc_path ${infile} PATH)
  SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/${outfilename}_res.o)
  #  parse file for dependencies
  FILE(READ "${infile}" _RC_FILE_CONTENTS)
  STRING(REGEX MATCHALL "<file>[^<]*" _RC_FILES "${_RC_FILE_CONTENTS}")
  SET(_RC_DEPENDS)
  FOREACH(_RC_FILE ${_RC_FILES})
    STRING(REGEX REPLACE "^<file>" "" _RC_FILE "${_RC_FILE}")
    SET(_RC_DEPENDS ${_RC_DEPENDS} "${rc_path}/${_RC_FILE}")
  ENDFOREACH(_RC_FILE)
  ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
    COMMAND windres
    ARGS -i ${infile} -o ${outfile} --include-dir=${CMAKE_CURRENT_SOURCE_DIR}
    MAIN_DEPENDENCY ${infile}
    DEPENDS ${_RC_DEPENDS})
  SET(${outfiles} ${${outfiles}} ${outfile})
ENDFOREACH (it)

ENDMACRO (QT4_ADD_RESOURCES2)
ENDIF(MINGW)

if (QTONLY)
  # ce: don't know why this is needed here - on win32 
  # 'O2' is activated by default in release mode
  if(NOT WIN32)
    add_definitions (${QT_DEFINITIONS} -O3)
  endif(NOT WIN32)

  include_directories(
    ${QT_INCLUDE_DIR}
  )
else (QTONLY)
  if(NOT WIN32)
    add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -O3) 
  endif(NOT WIN32)

  include_directories(
    ${KDE4_INCLUDES}
  )

endif (QTONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-marble.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-marble.h
)
