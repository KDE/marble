PROJECT(MarbleNavigatorPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/designer/navigator
 ${CMAKE_BINARY_DIR}/src/plugins/designer/navigator
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set( marblenavigator_SRCS MarbleNavigatorPlugin.cpp )
set( marblenavigator_HDRS MarbleNavigatorPlugin.h )

qt4_add_resources(marblenavigator_SRCS ../../../marble.qrc)

if (QTONLY)
  qt4_automoc(${marblenavigator_SRCS})
  add_library(MarbleNavigatorPlugin SHARED ${marblenavigator_SRCS})
  target_link_libraries (MarbleNavigatorPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS MarbleNavigatorPlugin DESTINATION ${QT_PLUGINS_DIR}/designer)
else (QTONLY)
  kde4_add_library(MarbleNavigatorPlugin SHARED ${marblenavigator_SRCS})
  target_link_libraries (MarbleNavigatorPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS MarbleNavigatorPlugin DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer)
endif (QTONLY)

set_target_properties(MarbleNavigatorPlugin PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE  SKIP_BUILD_RPATH TRUE BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX})
