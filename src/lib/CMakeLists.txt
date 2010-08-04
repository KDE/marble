PROJECT(marblewidget)

if (QTONLY)
   cmake_minimum_required(VERSION 2.4.0)
endif (QTONLY)


INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/lib
 ${CMAKE_BINARY_DIR}/src/lib
 ${CMAKE_CURRENT_SOURCE_DIR}/Projections
 ${CMAKE_CURRENT_SOURCE_DIR}/routing
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/data
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/graphicsitem
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/scene
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/handlers
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/handlers/gpx
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/handlers/kml
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/handlers/dgml
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/parser
 ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview
 ${QT_INCLUDE_DIR}
)
INCLUDE(${QT_USE_FILE})
INCLUDE(geodata/CMakeLists.txt)
INCLUDE(graphicsview/CMakeLists.txt)

set(GENERIC_LIB_VERSION "0.11.0")
set(GENERIC_LIB_SOVERSION "11")

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
    ${geodata_SRCS}
    ${graphicsview_SRCS}
    ${screengraphicsitem_SRCS}
    blendings/Blending.cpp
    blendings/BlendingAlgorithms.cpp
    blendings/BlendingFactory.cpp
    blendings/SunLightBlending.cpp
    DownloadRegionDialog.cpp
    LatLonBoxWidget.cpp
    MarbleWidget.cpp
    MarbleModel.cpp
    MarbleMap.cpp
    MarbleControlBox.cpp
    MarbleNavigator.cpp
    MarbleLegendBrowser.cpp
    MarbleThemeSelectView.cpp
    MarbleSearchListView.cpp
    MarbleAboutDialog.cpp
    MarbleWidgetInputHandler.cpp
    MarbleWidgetPopupMenu.cpp
    MarblePlacemarkModel.cpp
    GeoDataTreeModel.cpp
    MarbleDataFacade.cpp
    MarbleDebug.cpp
    TextureTile.cpp
    TileCoordsPyramid.cpp
    TileLevelRangeWidget.cpp
    TileLoader.cpp
    QtMarbleConfigDialog.cpp
    ClipPainter.cpp
    DownloadPolicy.cpp
    DownloadQueueSet.cpp
    GeoPainter.cpp
    GeoPolygon.cpp
    HttpDownloadManager.cpp
    HttpJob.cpp
    NetworkPlugin.cpp
    LayerManager.cpp
    PluginManager.cpp
    MarbleCacheSettingsWidget.cpp
    AbstractFloatItem.cpp
    global.cpp
    MarbleDirs.cpp
    MarbleLocale.cpp
    MarblePhysics.cpp
    DeferredFlag.cpp
    TileCreatorDialog.cpp
    MapThemeManager.cpp
    ViewportParams.cpp
    ViewParams.cpp
    Projections/AbstractProjection.cpp
    Projections/SphericalProjection.cpp
    Projections/EquirectProjection.cpp
    Projections/MercatorProjection.cpp
    MeasureTool.cpp
    ImageLoadThread.cpp
    VisiblePlacemark.cpp
    PlacemarkManager.cpp
    PlacemarkLayout.cpp
    PlacemarkPainter.cpp
    PlacemarkInfoDialog.cpp
    Planet.cpp
    Quaternion.cpp
    TextureColorizer.cpp
    AbstractScanlineTextureMapper.cpp
    SphericalScanlineTextureMapper.cpp
    EquirectScanlineTextureMapper.cpp
    MercatorScanlineTextureMapper.cpp
    DiscCache.cpp
    ServerLayout.cpp
    StoragePolicy.cpp
    CacheStoragePolicy.cpp
    FileStoragePolicy.cpp
    FileStorageWatcher.cpp
    DatasetProvider.cpp
    AbstractTile.cpp
    StackedTile.cpp
    TileId.cpp
    StackedTileLoader.cpp
    TileLoaderHelper.cpp
    TileCreator.cpp
    TinyWebBrowser.cpp
    #jsonparser.cpp
    VectorComposer.cpp
    VectorMap.cpp
    FileLoader.cpp
    FileManager.cpp
    FileViewModel.cpp
    KmlFileViewItem.cpp
    gps/PositionTracking.cpp
    DataMigration.cpp

    AbstractDataPlugin.cpp
    AbstractDataPluginModel.cpp
    AbstractDataPluginItem.cpp
    AbstractWorkerThread.cpp

    PluginInterface.cpp
    NetworkPluginInterface.cpp
    LayerInterface.cpp
    RenderPlugin.cpp
    RenderPluginInterface.cpp
    PositionProviderPlugin.cpp
    PositionProviderPluginInterface.cpp
    MarblePluginSettingsWidget.cpp
    PluginAboutDialog.cpp
    PluginItemDelegate.cpp

    SunLocator.cpp
    ExtDateTime.cpp
    SunControlWidget.cpp
    MergedLayerDecorator.cpp

    MathHelper.cpp

    LatLonEdit.cpp

    MapThemeSortFilterProxyModel.cpp

    routing/AlternativeRoutesModel.cpp
    routing/RouteAnnotator.cpp
    routing/RouteSkeleton.cpp
    routing/RoutingModel.cpp
    routing/RoutingProxyModel.cpp
    routing/RoutingManager.cpp
    routing/RoutingLayer.cpp
    routing/RoutingInputWidget.cpp
    routing/RoutingWidget.cpp

    MarbleRunnerManager.cpp

    routing/AdjustNavigation.cpp

    RunnerPlugin.cpp
    MarbleAbstractRunner.cpp
    RunnerTask.cpp
)

set (marblewidget_UI
    LatLonBoxWidget.ui
    NavigationWidget.ui
    LegendWidget.ui
    MapViewWidget.ui
    CurrentLocationWidget.ui
    FileViewWidget.ui
    MarbleNavigator.ui
    MarbleViewSettingsWidget.ui
    MarbleNavigationSettingsWidget.ui
    MarbleCacheSettingsWidget.ui
    MarblePluginSettingsWidget.ui
    TileCreatorDialog.ui
    TileLevelRangeWidget.ui
    PlacemarkInfoDialog.ui
    MarbleAboutDialog.ui
    SunControlWidget.ui
    LatLonEdit.ui
    routing/RoutingWidget.ui
    DataMigrationWidget.ui
)

# FIXME: cleaner approach of src/lib/MarbleControlBox.* vs. marble.qrc
qt4_add_resources(marblewidget_SRCS libmarble.qrc ../marble.qrc)

qt4_wrap_ui(marblewidget_SRCS ${marblewidget_UI})
qt4_automoc(${marblewidget_SRCS})


# link with release version of Qt libs
ADD_LIBRARY(marblewidget SHARED ${marblewidget_SRCS} ${SOURCES_UI_HDRS})
TARGET_LINK_LIBRARIES (marblewidget ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTSCRIPT_LIBRARY} ${QT_QTMAIN_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )

if (APPLE)
  #defined in top level makefile
  TARGET_LINK_LIBRARIES(marblewidget ${MAC_EXTRA_LIBS} )
endif (APPLE)

if (CMAKE_SYSTEM_NAME MATCHES "SunOS")
  TARGET_LINK_LIBRARIES(marblewidget m)
endif (CMAKE_SYSTEM_NAME MATCHES "SunOS")

if(WIN32)
  TARGET_LINK_LIBRARIES(marblewidget ws2_32 imm32 winmm)
endif(WIN32)


set_target_properties(marblewidget  PROPERTIES
                                    VERSION ${GENERIC_LIB_VERSION}
                                    SOVERSION ${GENERIC_LIB_SOVERSION}
                                    COMPILE_FLAGS "-DKML_LAZY_IMP")
# needed for marble_export.h
set_target_properties(marblewidget PROPERTIES
  DEFINE_SYMBOL MAKE_MARBLE_LIB
)

# choose the correct target install library path
if(WIN32 AND QTONLY)
  install(TARGETS marblewidget RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
else(WIN32 AND QTONLY)
  if(APPLE AND QTONLY)
    install (TARGETS marblewidget LIBRARY DESTINATION
      ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/lib)

  else(APPLE AND QTONLY)
    if(QTONLY)
      install(TARGETS marblewidget LIBRARY DESTINATION ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX})
    else(QTONLY)
        install(TARGETS marblewidget  ${INSTALL_TARGETS_DEFAULT_ARGS})
    endif(QTONLY)
  endif(APPLE AND QTONLY)
endif(WIN32 AND QTONLY)

add_executable(geodatatest geodata/GeoDataTest.cpp)
set_target_properties( geodatatest PROPERTIES COMPILE_FLAGS "-DKML_LAZY_IMP" )

target_link_libraries (geodatatest marblewidget ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install (TARGETS geodatatest RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

########### install files ###############

if (APPLE AND QTONLY)
  # do nothing because we dont put .h files in the bundle
else (APPLE AND QTONLY)
  install( FILES
    ${graphicsview_HDRS}
    DownloadRegionDialog.h
    LatLonBoxWidget.h
    MarbleWidget.h
    MarbleMap.h
    MarbleModel.h
    MarbleControlBox.h
    MarbleNavigator.h
    MarbleDataFacade.h
    AbstractFloatItem.h
    MapThemeManager.h
    MarbleAboutDialog.h
    MarbleWidgetInputHandler.h
    TileCoordsPyramid.h
    TileLevelRangeWidget.h
    TinyWebBrowser.h
    QtMarbleConfigDialog.h
    global.h
    MarbleDirs.h
    GeoPainter.h
    TileCreatorDialog.h
    ViewportParams.h
    ViewParams.h
    Projections/AbstractProjection.h
    Projections/SphericalProjection.h
    Projections/EquirectProjection.h
    Projections/MercatorProjection.h
    Quaternion.h
    SunLocator.h
    ClipPainter.h
    geodata/data/GeoDataAbstractView.h
    geodata/data/GeoDataAccuracy.h
    geodata/data/GeoDataColorStyle.h
    geodata/data/GeoDataContainer.h
    geodata/data/GeoDataCoordinates.h
    geodata/data/GeoDataDocument.h
    geodata/data/GeoDataFeature.h
    geodata/data/GeoDataFolder.h
    geodata/data/GeoDataGeometry.h
    geodata/data/GeoDataHotSpot.h
    geodata/data/GeoDataIconStyle.h
    geodata/data/GeoDataLabelStyle.h
    geodata/data/GeoDataLatLonAltBox.h
    geodata/data/GeoDataLatLonBox.h
    geodata/data/GeoDataLinearRing.h
    geodata/data/GeoDataLineString.h
    geodata/data/GeoDataLineStyle.h
    geodata/data/GeoDataLod.h
    geodata/data/GeoDataLookAt.h
    geodata/data/GeoDataMultiGeometry.h
    geodata/data/GeoDataObject.h
    geodata/data/GeoDataPlacemark.h
    geodata/data/GeoDataPoint.h
    geodata/data/GeoDataPolygon.h
    geodata/data/GeoDataPolyStyle.h
    geodata/data/GeoDataRegion.h
    geodata/data/GeoDataStyle.h
    geodata/data/GeoDataStyleMap.h
    geodata/data/GeoDataStyleSelector.h
    geodata/data/Serializable.h
    geodata/geodata_export.h
    geodata/parser/GeoDocument.h
    TileCreator.h
    PluginInterface.h
    RenderPlugin.h
    RenderPluginInterface.h
    MarbleAbstractRunner.h
    RunnerPlugin.h
    LayerInterface.h
    PluginAboutDialog.h
    marble_export.h
    Planet.h

    AbstractDataPlugin.h
    AbstractDataPluginModel.h
    AbstractDataPluginItem.h
    AbstractWorkerThread.h

    LatLonEdit.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/marble
)
endif (APPLE AND QTONLY)
