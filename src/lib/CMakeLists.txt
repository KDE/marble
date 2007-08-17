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

  include_directories(
    ${QT_INCLUDE_DIR}
  )
else (QTONLY)
  if(NOT WIN32)
    add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -O2)
  endif(NOT WIN32)

  include_directories(
    ${KDE4_INCLUDES}
  )
endif (QTONLY)

# link_directories (${QT_LIBRARY_DIR})
########### next target ###############

set(marblewidget_SRCS
           MarbleWidget.cpp
           MarbleModel.cpp
           MarbleControlBox.cpp
           MarbleNavigator.cpp
           MarbleLegendBrowser.cpp
           MarbleThemeSelectView.cpp
           MarbleAboutDialog.cpp

           MarbleWidgetInputHandler.cpp
           MarbleWidgetPopupMenu.cpp

           ClipPainter.cpp
           GeoPoint.cpp
           GeoPolygon.cpp
           GridMap.cpp
           HttpDownloadManager.cpp
           HttpFetchFile.cpp
           CrossHairFloatItem.cpp
           CompassFloatItem.cpp
           MapScaleFloatItem.cpp
           MarbleDirs.cpp
           katlasflag.cpp
           TileCreatorDialog.cpp
           maptheme.cpp
           ViewParams.cpp
           MeasureTool.cpp
           placemark.cpp
           PlaceMarkContainer.cpp
           placemarkmanager.cpp
           placemarkpainter.cpp
           placemarkmodel.cpp
           placemarkinfodialog.cpp
           Quaternion.cpp
           searchcombobox.cpp
           searchlistview.cpp
           texcolorizer.cpp
           FlatScanlineTextureMapper.cpp
           GlobeScanlineTextureMapper.cpp
           AbstractScanlineTextureMapper.cpp
           TextureTile.cpp
           TileCache.cpp
           TileLoader.cpp
           TileCreator.cpp
           tinywebbrowser.cpp
           vectorcomposer.cpp
           vectormap.cpp
           xmlhandler.cpp
           BoundingBox.cpp
           kml/KMLContainer.cpp
           kml/KMLContainerParser.cpp
           kml/KMLFeature.cpp
           kml/KMLFeatureParser.cpp
           kml/KMLFolder.cpp
           kml/KMLDocument.cpp
           kml/KMLDocumentParser.cpp
           kml/KMLObject.cpp
           kml/KMLObjectParser.cpp
           kml/KMLPlaceMark.cpp
           kml/KMLPlaceMarkParser.cpp
           kml/KMLPointParser.cpp
           gps/GpsTracking.cpp
           gps/GpsElement.cpp
           gps/Waypoint.cpp
           gps/TrackPoint.cpp
           gps/Route.cpp
           gps/Track.cpp
           gps/TrackSegment.cpp
           gps/WaypointContainer.cpp
           gps/TrackContainer.cpp
           gps/RouteContainer.cpp
           gps/GpsLayer.cpp
           gps/GmlSax.cpp
           gps/GpxSax.cpp
           gps/GpxFile.cpp
           gps/GpxFileModel.cpp
	   AbstractLayer/AbstractLayerInterface.cpp
           AbstractLayer/AbstractLayer.cpp
           AbstractLayer/AbstractLayerContainer.cpp
           AbstractLayer/AbstractLayerData.cpp
)
MESSAGE( STATUS "${marblewidget_SRCS}" )

set (marblewidget_UI
    MarbleControlBox.ui
    MarbleNavigator.ui
    TileCreatorDialog.ui
    placemarkinfodialog.ui
    MarbleAboutDialog.ui
)
MESSAGE( STATUS "${marble_UI}" )

qt4_add_resources(marblewidget_SRCS libmarble.qrc)

if (QTONLY)
  qt4_wrap_ui(marblewidget_SRCS ${marblewidget_UI})
  qt4_automoc(${marblewidget_SRCS})
else (QTONLY)
  kde4_add_ui_files(marblewidget_SRCS ${marblewidget_UI})
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

if (APPLE)
  #defined in top level makefile
  TARGET_LINK_LIBRARIES(marblewidget ${MAC_EXTRA_LIBS} )
endif (APPLE)

set(HAVE_LIBGPS 0)

if(WIN32)
  TARGET_LINK_LIBRARIES(marblewidget ws2_32 imm32 winmm)
else(WIN32)
  FIND_PACKAGE(libgps)

  if(libgps_FOUND)
     set(HAVE_LIBGPS 1)
     message( STATUS "building with gpsd support")
     
     INCLUDE_DIRECTORIES(${libgps_INCLUDES})
     TARGET_LINK_LIBRARIES(marblewidget ${libgps_LIBRARIES})
  else(libgps_FOUND)
     message( STATUS "not building with gpsd support")
  endif(libgps_FOUND)


endif(WIN32)

configure_file(config-libgps.h.cmake
${CMAKE_CURRENT_BINARY_DIR}/config-libgps.h)

set_target_properties(marblewidget PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION})
# needed for marble_export.h
set_target_properties(marblewidget PROPERTIES
  DEFINE_SYMBOL MAKE_MARBLE_LIB
)

# choose the correct target install library path
if(WIN32 AND QTONLY)
  install(TARGETS marblewidget DESTINATION "")
else(WIN32 AND QTONLY)
  if(APPLE AND QTONLY)
    install (TARGETS marblewidget DESTINATION 
      ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/lib)

  else(APPLE AND QTONLY)
    if(QTONLY)
      install(TARGETS marblewidget DESTINATION lib)
    else(QTONLY)
      install(TARGETS marblewidget DESTINATION ${LIB_INSTALL_DIR})
    endif(QTONLY)
  endif(APPLE AND QTONLY)
endif(WIN32 AND QTONLY)

########### install files ###############

install( FILES
           MarbleWidget.h
           MarbleModel.h
           MarbleControlBox.h
           MarbleNavigator.h
           MarbleLegendBrowser.h
           MarbleThemeSelectView.h
           MarbleAboutDialog.h

           MarbleWidgetInputHandler.h
           MarbleWidgetPopupMenu.h

           global.h
           ClipPainter.h
           GeoPoint.h
           GeoPolygon.h
           GridMap.h
           HttpDownloadManager.h
           HttpFetchFile.h
           CrossHairFloatItem.h
           CompassFloatItem.h
           MapScaleFloatItem.h
           MarbleDirs.h
           katlasflag.h
           TileCreatorDialog.h
           maptheme.h
           ViewParams.h
           MeasureTool.h
           placemark.h
           PlaceMarkContainer.h
           placemarkmanager.h
           placemarkpainter.h
           placemarkmodel.h
           placemarkinfodialog.h
           Quaternion.h
           ScreenPolygon.h
           searchcombobox.h
           searchlistview.h
           texcolorizer.h
           FlatScanlineTextureMapper.h
           GlobeScanlineTextureMapper.h
           AbstractScanlineTextureMapper.h
           TextureTile.h
           TileCache.h
           TileLoader.h
           TileCreator.h
           tinywebbrowser.h
           vectorcomposer.h
           vectormap.h
           xmlhandler.h
           marble_export.h
           DESTINATION ${CMAKE_INSTALL_PREFIX}/include/marble
)
