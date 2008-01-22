PROJECT(marblewidget)
cmake_minimum_required(VERSION 2.4.0)


INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/lib
 ${CMAKE_BINARY_DIR}/src/lib
 ${CMAKE_CURRENT_SOURCE_DIR}/AbstractLayer
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/data
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/handlers/gpx
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/handlers/kml
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/parser
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})

INCLUDE(geodata/CMakeLists.txt)

if (QTONLY)
  set(GENERIC_LIB_VERSION "0.5.0")
  set(GENERIC_LIB_SOVERSION "5")
endif (QTONLY)

if (QTONLY)
  # ce: don't know why this is needed here - on win32 'O2' is activated by default in release mode
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

# link_directories (${QT_LIBRARY_DIR})
########### next target ###############

set(marblewidget_SRCS
#           ${geodata_SRCS}
           MarbleWidget.cpp
           MarbleModel.cpp
           MarbleControlBox.cpp
           MarbleNavigator.cpp
           MarbleLegendBrowser.cpp
           MarbleThemeSelectView.cpp
           MarbleSearchListView.cpp
           MarbleAboutDialog.cpp
           MarbleWidgetInputHandler.cpp
           MarbleWidgetPopupMenu.cpp
           MarblePlacemarkModel.cpp

           AutoSettings.cpp
           ClipPainter.cpp
           GeoPolygon.cpp
           GridMap.cpp
           HttpDownloadManager.cpp
           HttpFetchFile.cpp
           CrossHairFloatItem.cpp
           CompassFloatItem.cpp
           MapScaleFloatItem.cpp
           MarbleDirs.cpp
           DeferredFlag.cpp
           TileCreatorDialog.cpp
           MapTheme.cpp
           ViewParams.cpp
           MeasureTool.cpp
           VisiblePlaceMark.cpp
           PlaceMarkContainer.cpp
           PlaceMarkManager.cpp
           PlaceMarkLayout.cpp
           PlaceMarkPainter.cpp
           PlaceMarkInfoDialog.cpp
           Quaternion.cpp
           TextureColorizer.cpp
           FlatScanlineTextureMapper.cpp
           GlobeScanlineTextureMapper.cpp
           AbstractScanlineTextureMapper.cpp
           DiscCache.cpp
           CacheStoragePolicy.cpp
           FileStoragePolicy.cpp
           TextureTile.cpp
           TileCache.cpp
           TileLoader.cpp
           TileCreator.cpp
           TinyWebBrowser.cpp
           VectorComposer.cpp
           VectorMap.cpp
           XmlHandler.cpp
           BoundingBox.cpp
           FileViewModel.cpp
           GpxFileViewItem.cpp
           KMLFileViewItem.cpp
           geodata/data/GeoDataColorStyle.cpp
           geodata/data/GeoDataContainer.cpp
           geodata/data/GeoDataDocument.cpp
           geodata/data/GeoDataFeature.cpp
           geodata/data/GeoDataFolder.cpp
           geodata/data/GeoDataHotSpot.cpp
           geodata/data/GeoDataIconStyle.cpp
           geodata/data/GeoDataLabelStyle.cpp
           geodata/data/GeoDataObject.cpp
           geodata/data/GeoDataPlacemark.cpp
           geodata/data/GeoDataPoint.cpp
           geodata/data/GeoDataStyle.cpp
           geodata/data/GeoDataStyleSelector.cpp
#           kml/KMLColorStyle.cpp
#           kml/KMLColorStyleParser.cpp
#           kml/KMLContainer.cpp
#           kml/KMLContainerParser.cpp
#           kml/KMLFeature.cpp
#           kml/KMLFeatureParser.cpp
#           kml/KMLFolder.cpp
#           kml/KMLDocument.cpp
#           kml/KMLDocumentParser.cpp
#           kml/KMLIconStyle.cpp
#           kml/KMLIconStyleParser.cpp
#           kml/KMLLabelStyle.cpp
#           kml/KMLLabelStyleParser.cpp
#           kml/KMLObject.cpp
#           kml/KMLObjectParser.cpp
#           kml/KMLGeoDataPlaceMark.cpp
#           kml/KMLPlaceMarkParser.cpp
#           kml/KMLPointParser.cpp
#           kml/KMLStyle.cpp
#           kml/KMLStyleParser.cpp
#           kml/KMLStyleSelector.cpp
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

           SunLocator.cpp
           ExtDateTime.cpp
           SunControlWidget.cpp
           MergedLayerPainter.cpp
)

set (marblewidget_UI
    MarbleControlBox.ui
    MarbleNavigator.ui
    TileCreatorDialog.ui
    PlaceMarkInfoDialog.ui
    MarbleAboutDialog.ui
    SunControlWidget.ui
)

qt4_add_resources(marblewidget_SRCS libmarble.qrc)

if (QTONLY)
  qt4_wrap_ui(marblewidget_SRCS ${marblewidget_UI})
  qt4_automoc(${marblewidget_SRCS})
else (QTONLY)
  kde4_add_ui_files(marblewidget_SRCS ${marblewidget_UI})
endif (QTONLY)

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
      install(TARGETS marblewidget DESTINATION ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX})
    else(QTONLY)
        install(TARGETS marblewidget  DESTINATION ${LIB_INSTALL_DIR})
    endif(QTONLY)
  endif(APPLE AND QTONLY)
endif(WIN32 AND QTONLY)

########### install files ###############

if (APPLE AND QTONLY)
  # do nothing because we dont put .h files in the bundle
else (APPLE AND QTONLY)
  install( FILES
           MarbleWidget.h
           MarbleModel.h
           MarbleControlBox.h
           MarbleNavigator.h
           MarbleLegendBrowser.h
           MarbleThemeSelectView.h
           MarbleSearchListView.h
           MarbleAboutDialog.h
           SunControlWidget.h

           MarbleWidgetInputHandler.h
           MarbleWidgetPopupMenu.h

           global.h
           AutoSettings.h
           BoundingBox.h
           ClipPainter.h
           GeoPolygon.h
           GridMap.h
           HttpDownloadManager.h
           HttpFetchFile.h
           CrossHairFloatItem.h
           CompassFloatItem.h
           MapScaleFloatItem.h
           MarbleDirs.h
           DeferredFlag.h
           TileCreatorDialog.h
           MapTheme.h
           ViewParams.h
           MeasureTool.h
           VisiblePlaceMark.h
           PlaceMarkContainer.h
           PlaceMarkManager.h
           PlaceMarkLayout.h
           PlaceMarkPainter.h
           MarblePlacemarkModel.h
           PlaceMarkInfoDialog.h
           Quaternion.h
           ScreenPolygon.h
           TextureColorizer.h
           FlatScanlineTextureMapper.h
           GlobeScanlineTextureMapper.h
           AbstractScanlineTextureMapper.h
           TextureTile.h
           TileCache.h
           TileLoader.h
           TileCreator.h
           TinyWebBrowser.h
           VectorComposer.h
           VectorMap.h
           XmlHandler.h
           marble_export.h
           SunLocator.h
           ExtDateTime.h
           MergedLayerPainter.h

           AbstractLayer/AbstractLayer.h
           AbstractLayer/AbstractLayerContainer.h
           AbstractLayer/AbstractLayerData.h
           AbstractLayer/AbstractLayerInterface.h

           geodata/data/Serializable.h
           geodata/data/GeoDataObject.h
           geodata/data/GeoDataPoint.h
           geodata/data/GeoDataPlacemark.h
           geodata/data/GeoDataFeature.h
           DESTINATION ${CMAKE_INSTALL_PREFIX}/include/marble
)
endif (APPLE AND QTONLY)
