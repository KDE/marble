
PROJECT(MarbleWidgetPlugin)

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/plugins/marblewidget
 ${CMAKE_BINARY_DIR}/src/plugins/marblewidget
 ${QT_INCLUDE_DIR}
 ${QT_QTDESIGNER_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

set(marbleplugin_SRCS
           MarbleWidgetPlugin.cpp
)
MESSAGE( STATUS ${marbleplugin_SRCS} )

set(marbleplugin_HDRS
           MarbleWidgetPlugin.h
)
MESSAGE( STATUS ${marbleplugin_HDRS} )

qt4_add_resources(marbleplugin_SRCS ../../marble.qrc)

if (QTONLY)
  qt4_automoc(${marbleplugin_SRCS})
else (QTONLY)
  kde4_automoc(${marbleplugin_SRCS})
endif (QTONLY)

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_DLL)

ADD_LIBRARY(MarbleWidgetPlugin SHARED ${marbleplugin_SRCS})

if (QTONLY)
    TARGET_LINK_LIBRARIES (MarbleWidgetPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
else (QTONLY)
    TARGET_LINK_LIBRARIES (MarbleWidgetPlugin ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY} marblewidget)
endif (QTONLY)
if(WIN32)
  TARGET_LINK_LIBRARIES(MarbleWidgetPlugin ws2_32 imm32 winmm)
endif(WIN32)

install(TARGETS MarbleWidgetPlugin DESTINATION ${QT_PLUGINS_DIR}/designer)

