
PROJECT(MarbleTestPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/marble/test
 ${CMAKE_BINARY_DIR}/src/plugins/marble/test
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set(marbletest_SRCS
           MarbleTestPlugin.cpp
)
MESSAGE( STATUS ${marbletest_SRCS} )

set(marbletest_HDRS
           MarbleTestPlugin.h
)
MESSAGE( STATUS ${marbletest_HDRS} )

if(WIN32)
  # distinguish between debug and release plugin
  SET(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)

if (QTONLY)
  qt4_automoc(${marbletest_SRCS})
  add_library(MarbleTestPlugin SHARED ${marbletest_SRCS})
  target_link_libraries (MarbleTestPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS MarbleTestPlugin DESTINATION ${MARBLE_DATA_INSTALL_PATH}/plugins)
else (QTONLY)
  kde4_add_library(MarbleTestPlugin SHARED ${marbletest_SRCS})
  target_link_libraries (MarbleTestPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
  install(TARGETS MarbleTestPlugin DESTINATION ${MARBLE_DATA_INSTALL_PATH}/plugins)
endif (QTONLY)

set_target_properties(MarbleTestPlugin PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE  SKIP_BUILD_RPATH TRUE BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${LIB_SUFFIX})
