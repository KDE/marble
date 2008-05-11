
PROJECT(MarbleTestPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/satellites
 ${CMAKE_BINARY_DIR}/src/plugins/marble/satellites
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set(satellites_SRCS
           SatellitesPlugin.cpp
)
MESSAGE( STATUS ${satellites_SRCS} )

set(satellites_HDRS
           SatellitesPlugin.h
)
MESSAGE( STATUS ${satellites_HDRS} )

if(WIN32)
  # distinguish between debug and release plugin
  SET(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

if (QTONLY)
  qt4_automoc(${satellites_SRCS})
  add_library(MarbleTestPlugin SHARED ${satellites_SRCS})
  target_link_libraries (MarbleTestPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS MarbleTestPlugin DESTINATION ${MARBLE_DATA_INSTALL_PATH}/plugins)
else (QTONLY)
  kde4_add_library(MarbleTestPlugin SHARED ${satellites_SRCS})
  target_link_libraries (MarbleTestPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS MarbleTestPlugin DESTINATION ${MARBLE_DATA_INSTALL_PATH}/plugins)
endif (QTONLY)

set_target_properties(MarbleTestPlugin PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE  SKIP_BUILD_RPATH TRUE BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX})
