PROJECT(CompassFloatItem)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/compass
 ${CMAKE_BINARY_DIR}/src/plugins/marble/compass
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( compass_SRCS CompassFloatItem.cpp )
set( compass_HDRS CompassFloatItem.h )

if (QTONLY)
  qt4_automoc(${compass_SRCS})
  add_library(CompassFloatItem SHARED ${compass_SRCS})
  target_link_libraries (CompassFloatItem ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS CompassFloatItem DESTINATION ${MARBLE_PLUGIN_INSTALL_PATH})
else (QTONLY)
  kde4_add_library(CompassFloatItem SHARED ${compass_SRCS})
  target_link_libraries (CompassFloatItem ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS CompassFloatItem DESTINATION ${MARBLE_PLUGIN_INSTALL_PATH})
endif (QTONLY)

set_target_properties(CompassFloatItem PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE  SKIP_BUILD_RPATH TRUE BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX})
