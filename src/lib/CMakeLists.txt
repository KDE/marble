
PROJECT(marblewidget)
cmake_minimum_required(VERSION 2.4.0)

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/lib
 ${CMAKE_BINARY_DIR}/src/lib
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

if (QTONLY)
  set(GENERIC_LIB_VERSION "5.0.0")
  set(GENERIC_LIB_SOVERSION "5")
endif (QTONLY)


# link_directories (${QT_LIBRARY_DIR})
########### next target ###############

set(marblewidget_SRCS
           clippainter.cpp
           GeoPoint.cpp
           GeoPolygon.cpp
           GridMap.cpp
           HttpDownloadManager.cpp
           httpfetchfile.cpp
           katlasaboutdialog.cpp
#           katlascontrol.cpp
           katlascrosshair.cpp
           katlasflag.cpp
           katlasglobe.cpp
           katlasmapscale.cpp
           katlasthemeselectview.cpp
           katlastilecreatordialog.cpp
           katlasviewinputhandler.cpp
           katlasviewpopupmenu.cpp
           katlaswindrose.cpp
           maptheme.cpp
#           marblecontrol.cpp
           MarbleWidget.cpp
           measuretool.cpp
           placecontainer.cpp
           placemark.cpp
           placemarkinfodialog.cpp
           placemarkmanager.cpp
           placemarkmodel.cpp
           placemarkpainter.cpp
           Quaternion.cpp
           searchcombobox.cpp
           searchlistview.cpp
           texcolorizer.cpp
           TextureMapper.cpp
           TextureTile.cpp
           TileLoader.cpp
           tilescissor.cpp
           tinywebbrowser.cpp
           vectorcomposer.cpp
           vectormap.cpp
           xmlhandler.cpp
)
MESSAGE( STATUS ${marblewidget_SRCS} )

#if (QTONLY)
#  set(marble_SRCS ${marble_SRCS} qtmain.cpp QtMainWindow.cpp)
#  set(marble_HDRS ${marble_HDRS} QtMainWindow.h)
#else (QTONLY)
#  set(marble_SRCS ${marble_SRCS} kdemain.cpp KdeMainWindow.cpp)
#  set(marble_HDRS ${marble_HDRS} KdeMainWindow.h)
#endif (QTONLY)

#if(MSVC)
#  set(marble_SRCS ${marble_SRCS} marble.rc)
#endif(MSVC)

#if(MINGW)
#   QT4_ADD_RESOURCES2(marble_SRCS marble.rc)
#endif(MINGW)

set (marblewidget_UI katlastilecreatordialog.ui placemarkinfodialog.ui katlasaboutdialog.ui
#           marblecontrol.ui	   MarbleControlBox.ui 
)
MESSAGE( STATUS ${marble_UI} )

qt4_add_resources(marblewidget_SRCS libmarble.qrc)

if (QTONLY)
  qt4_wrap_ui(SOURCES_UI_HDRS ${marblewidget_UI})
  qt4_automoc(${marblewidget_SRCS})
else (QTONLY)
  kde4_add_ui_files(marblewidget_SRCS ${marblewidget_UI})
  kde4_automoc(${marblewidget_SRCS})
endif (QTONLY)

# qt4_generate_moc(${CMAKE_SOURCE_DIR}/src/MarbleWidgetPlugin.h ${CMAKE_CURRENT_BINARY_DIR}/src/MarbleWidgetPlugin.moc)


#kde4_add_library (marble SHARED ${marble_SRCS})
#if (QTONLY)
#  add_executable (marble WIN32 ${marble_SRCS} ${marble_HDRS})
#else (QTONLY)
#  kde4_add_executable (marble ${marble_SRCS} ${marble_HDRS})
#endif (QTONLY)

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_DLL)

ADD_LIBRARY(marblewidget SHARED ${marblewidget_SRCS} ${SOURCES_UI_HDRS})

# link with release version of Qt libs

if (QTONLY)
    TARGET_LINK_LIBRARIES (marblewidget ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY})
else (QTONLY)
    TARGET_LINK_LIBRARIES (marblewidget ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY})
endif (QTONLY)
if(WIN32)
  TARGET_LINK_LIBRARIES(marblewidget ws2_32 imm32 winmm)
endif(WIN32)

# set_target_properties(marblewidget PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})

install(TARGETS marblewidget DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

########### install files ###############

install( FILES
           clippainter.h
           GeoPoint.h
           GeoPolygon.h
           GridMap.h
           HttpDownloadManager.h
           httpfetchfile.h
           katlasaboutdialog.h
           katlascontrol.h
           katlascrosshair.h
           katlasdirs.h
           katlasflag.h
           katlasglobe.h
           katlasmapscale.h
           katlasthemeselectview.h
           katlastilecreatordialog.h
           katlasviewinputhandler.h
           katlasviewpopupmenu.h
           katlaswindrose.h
           maptheme.h
           marblecontrol.h
           MarbleWidget.h
           measuretool.h
           placecontainer.h
           placemark.h
           placemarkinfodialog.h
           placemarkmanager.h
           placemarkmodel.h
           placemarkpainter.h
           Quaternion.h
           ScreenPolygon.h
           searchcombobox.h
           searchlistview.h
           texcolorizer.h
           TextureMapper.h
           TextureTile.h
           TileLoader.h
           tilescissor.h
           tinywebbrowser.h
           vectorcomposer.h
           vectormap.h
           xmlhandler.h
           DESTINATION ${CMAKE_INSTALL_PREFIX}/include/marble)


