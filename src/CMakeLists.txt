
# before add_subdirectory() !
if(MSVC)
  add_definitions(-D_USE_MATH_DEFINES=1)
endif(MSVC)

if(BUILD_WITH_DBUS)
add_definitions(-DMARBLE_DBUS)
endif(BUILD_WITH_DBUS)

if( QT_QTDECLARATIVE_FOUND )
  OPTION(IGNORE_CMAKE_INSTALL_PREFIX_FOR_DECLARATIVE_PLUGINS "Install Marble Qt Declarative plugins to QT_IMPORTS_DIR even if that is out of the CMAKE_INSTALL_PREFIX")
  IF(DEFINED QT_IMPORTS_DIR)
    SET(MARBLE_QT_IMPORTS_DIR ${QT_IMPORTS_DIR})
  ELSE()
    # Fallback when FindQt4.cmake (part of cmake) does not define QT_IMPORTS_DIR. Get it from qmake instead.
    EXEC_PROGRAM( ${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_IMPORTS" OUTPUT_VARIABLE MARBLE_QT_IMPORTS_DIR )
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

add_subdirectory( tilecreator )
add_subdirectory( routing-instructions )
add_subdirectory( lib )
add_subdirectory( plugins )
add_subdirectory( bindings )

if( QT_QTDECLARATIVE_FOUND )
    add_subdirectory( qt-components )
endif( QT_QTDECLARATIVE_FOUND )

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

set(marble_SRCS
    ControlView.cpp
    MarbleTest.cpp
)

set(marble_HDRS
    ControlView.h
    MarbleTest.h
)

if (QTONLY)
  set(marble_SRCS ${marble_SRCS} qtmain.cpp QtMainWindow.cpp StackableWindow.cpp)
  set(marble_HDRS ${marble_HDRS} QtMainWindow.h StackableWindow.h)
else (QTONLY)
  set(marble_SRCS ${marble_SRCS} kdemain.cpp KdeMainWindow.cpp marble_part.cpp)
  set(marble_HDRS ${marble_HDRS} KdeMainWindow.h)
endif (QTONLY)

if (NOT QTONLY)
  set(marblepart_SRCS marble_part.cpp)
  kde4_add_kcfg_files(marblepart_SRCS settings.kcfgc)
  kde4_add_plugin(marble_part WITH_PREFIX ${marblepart_SRCS} ControlView.cpp)
  target_link_libraries(marble_part ${KDE4_KPARTS_LIBS} ${KDE4_KNEWSTUFF3_LIBS} marblewidget)

  install(TARGETS marble_part DESTINATION ${PLUGIN_INSTALL_DIR})
  install(FILES marble_part.desktop DESTINATION ${SERVICES_INSTALL_DIR})
  install(FILES marble_part.rc DESTINATION ${DATA_INSTALL_DIR}/marble)
endif (NOT QTONLY)

if(MSVC)
  set(marble_SRCS ${marble_SRCS} marble.rc)
endif(MSVC)

if(MINGW)
   QT4_ADD_RESOURCES2(marble_SRCS marble.rc)
endif(MINGW)

qt4_add_resources(marble_SRCS marble.qrc)

if (QTONLY)
  qt4_wrap_ui(marble_SRCS ${marble_UI})
  qt4_automoc(${marble_SRCS})
else (QTONLY)
  kde4_add_ui_files(marble_SRCS ${marble_UI})
  kde4_add_kcfg_files(marble_SRCS settings.kcfgc)
endif (QTONLY)


if (QTONLY)
    if (WIN32)
        add_executable (marble WIN32 ${marble_SRCS} ${marble_HDRS})
    else (WIN32)
        if (APPLE)
            add_executable (marble MACOSX_BUNDLE ${marble_SRCS})
            # For Mac OS X, the executable must be at the root of the bundle's executable folder
            INSTALL(TARGETS marble RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
        else (APPLE)
            add_executable (marble ${marble_SRCS} ${marble_HDRS})
        endif (APPLE)
    endif (WIN32)
else (QTONLY)
  kde4_add_executable (marble ${marble_SRCS} ${marble_HDRS})
endif (QTONLY)

if (QTONLY)
    target_link_libraries (
      marble 
      ${QT_QTCORE_LIBRARY} 
      ${QT_QTDBUS_LIBRARY} 
      ${QT_QTGUI_LIBRARY} 
      ${QT_QTXML_LIBRARY} 
      ${QT_QTSVG_LIBRARY} 
      ${QT_QTNETWORK_LIBRARY}
      ${QT_QTSCRIPT_LIBRARY} 
      ${QT_QTMAIN_LIBRARY} 
      marblewidget)
else (QTONLY)
    target_link_libraries (
      marble 
      ${KDE4_KPARTS_LIBS} 
      ${KDE4_KNEWSTUFF3_LIBS}
      marblewidget
      )
endif (QTONLY)
if(WIN32)
  target_link_libraries(
    marble 
    ws2_32 
    imm32 
    winmm
    )
endif(WIN32)

## this works for win32 only because FindQt4 does not handle this correct
if(STATIC_BUILD)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qjpeg.lib)
    target_link_libraries(marble ${QT_PLUGINS_DIR}/imageformats/qsvg.lib)
endif(STATIC_BUILD)

if(WIN32 AND QTONLY)
    install (TARGETS marble RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
else(WIN32 AND QTONLY)
    if (APPLE AND QTONLY)
      # No need for this when installing to a bundle
    else (APPLE AND QTONLY)
      #set_target_properties(marble PROPERTIES VERSION 1.0.0 SOVERSION 1)
      if( NOT QTONLY)
        install (TARGETS marble ${INSTALL_TARGETS_DEFAULT_ARGS})
        install( PROGRAMS marble.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
        install(FILES marble.kcfg DESTINATION ${KCFG_INSTALL_DIR})
      else( NOT QTONLY)
        install (TARGETS marble RUNTIME DESTINATION bin )
        install( FILES marble.desktop  DESTINATION share/applications)
      endif(NOT QTONLY)
      #install (TARGETS marble ${INSTALL_TARGETS_DEFAULT_ARGS} )
    endif (APPLE AND QTONLY)
endif(WIN32 AND QTONLY)

# install the XML GUI file and the Hot New Stuff config
if( NOT QTONLY )
    install( FILES marbleui.rc DESTINATION ${DATA_INSTALL_DIR}/marble )
    install( FILES marble.knsrc  DESTINATION ${DATA_INSTALL_DIR}/marble )
endif( NOT QTONLY )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config-marble.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/config-marble.h
)
