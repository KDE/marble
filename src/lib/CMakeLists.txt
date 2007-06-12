
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

if (QTONLY)
  # ce: don't know why this is needed here - on win32 'O2' is activated by default in release mode
  if(NOT WIN32)
    add_definitions (${QT_DEFINITIONS} -O2)
  endif(NOT WIN32)
  include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${QT_INCLUDE_DIR})
else (QTONLY)
  if(NOT WIN32)
    add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -O2)
  endif(NOT WIN32)
  include_directories (${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src ${KDE4_INCLUDES})
endif (QTONLY)

# link_directories (${QT_LIBRARY_DIR})
########### next target ###############

set(marblewidget_SRCS
           AbstractLayer.cpp
           clippainter.cpp
           GeoPoint.cpp
           GeoPolygon.cpp
           GridMap.cpp
           HttpDownloadManager.cpp
           httpfetchfile.cpp
           katlasaboutdialog.cpp
           katlascrosshair.cpp
           katlasflag.cpp
           katlasmapscale.cpp
           katlasthemeselectview.cpp
           katlastilecreatordialog.cpp
           katlasviewinputhandler.cpp
           katlasviewpopupmenu.cpp
           katlaswindrose.cpp
           LegendBrowser.cpp
           maptheme.cpp
           MarbleModel.cpp
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
           kml/KMLFeature.cpp
           kml/KMLFolder.cpp
           kml/KMLDocument.cpp
           kml/KMLObject.cpp
)
MESSAGE( STATUS "${marblewidget_SRCS}" )

set (marblewidget_UI katlastilecreatordialog.ui placemarkinfodialog.ui katlasaboutdialog.ui
)
MESSAGE( STATUS "${marble_UI}" )

qt4_add_resources(marblewidget_SRCS libmarble.qrc)

if (QTONLY)
  qt4_wrap_ui(marblewidget_SRCS ${marblewidget_UI})
  qt4_automoc(${marblewidget_SRCS})
else (QTONLY)
  kde4_add_ui_files(marblewidget_SRCS ${marblewidget_UI})
  kde4_automoc(${marblewidget_SRCS})
endif (QTONLY)

# why this??
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_DLL)
if(WIN32)
  # distinguish between debug and release plugin
  SET(CMAKE_DEBUG_POSTFIX "d")
endif(WIN32)


# link with release version of Qt libs

if (QTONLY)
    ADD_LIBRARY(marblewidget SHARED ${marblewidget_SRCS} ${SOURCES_UI_HDRS})
    TARGET_LINK_LIBRARIES (marblewidget ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTMAIN_LIBRARY})
else (QTONLY)
    KDE4_ADD_LIBRARY(marblewidget SHARED ${marblewidget_SRCS} ${SOURCES_UI_HDRS})
    TARGET_LINK_LIBRARIES (marblewidget ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY})
endif (QTONLY)
if(WIN32)
  TARGET_LINK_LIBRARIES(marblewidget ws2_32 imm32 winmm)
endif(WIN32)

# set_target_properties(marblewidget PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
# needed for marble_export.h
set_target_properties(marblewidget PROPERTIES
  DEFINE_SYMBOL MAKE_MARBLE_LIB
)

# chopse the correct target install library path
if(WIN32 AND QTONLY)
  install(TARGETS marblewidget DESTINATION "")
endif(WIN32 AND QTONLY)

if(QTONLY)
  install(TARGETS marblewidget DESTINATION lib)
else(QTONLY)
  install(TARGETS marblewidget DESTINATION ${LIB_INSTALL_DIR})
endif(QTONLY)

########### install files ###############

install( FILES
           AbstractLayer.h
           clippainter.h
           GeoPoint.h
           GeoPolygon.h
           GridMap.h
           HttpDownloadManager.h
           httpfetchfile.h
           katlasaboutdialog.h
           katlascrosshair.h
           katlasdirs.h
           katlasflag.h
           katlasmapscale.h
           katlasthemeselectview.h
           katlastilecreatordialog.h
           katlasviewinputhandler.h
           katlasviewpopupmenu.h
           katlaswindrose.h
           LegendBrowser.h
           maptheme.h
           MarbleModel.h
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
           marble_export.h
           DESTINATION ${CMAKE_INSTALL_PREFIX}/include/marble
)


