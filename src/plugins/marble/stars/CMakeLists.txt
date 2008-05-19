
PROJECT(MarbleStarsPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/stars
 ${CMAKE_BINARY_DIR}/src/plugins/marble/stars
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set(marblestars_SRCS
           MarbleStarsPlugin.cpp
)
MESSAGE( STATUS ${marblestars_SRCS} )

set(marblestars_HDRS
           MarbleStarsPlugin.h
)
MESSAGE( STATUS ${marblestars_HDRS} )

if(WIN32)
  # distinguish between debug and release plugin
  SET(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

if (QTONLY)
  qt4_automoc(${marblestars_SRCS})
  add_library(MarbleStarsPlugin SHARED ${marblestars_SRCS})
  target_link_libraries (MarbleStarsPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS MarbleStarsPlugin DESTINATION ${MARBLE_PLUGIN_INSTALL_PATH})
else (QTONLY)
  kde4_add_library(MarbleStarsPlugin SHARED ${marblestars_SRCS})
  target_link_libraries (MarbleStarsPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS MarbleStarsPlugin DESTINATION ${MARBLE_PLUGIN_INSTALL_PATH})
endif (QTONLY)

set_target_properties(MarbleStarsPlugin PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE  SKIP_BUILD_RPATH TRUE BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX})
