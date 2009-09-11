PROJECT(marblewidget)

if (QTONLY)
   cmake_minimum_required(VERSION 2.4.0)
endif (QTONLY)


INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}/src/lib
 ${CMAKE_BINARY_DIR}/src/lib
 ${CMAKE_CURRENT_SOURCE_DIR}/AbstractLayer
 ${CMAKE_CURRENT_SOURCE_DIR}/Projections
 ${CMAKE_CURRENT_SOURCE_DIR}/runners
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

if (QTONLY)
  set(GENERIC_LIB_VERSION "0.9.0")
  set(GENERIC_LIB_SOVERSION "9")
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
    ${geodata_SRCS}
    ${graphicsview_SRCS}
    ${screengraphicsitem_SRCS}
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
    MarbleGeoDataModel.cpp
    MarbleGeometryModel.cpp
    MarbleDataFacade.cpp

    QtMarbleConfigDialog.cpp
    ClipPainter.cpp
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
    StoragePolicy.cpp
    CacheStoragePolicy.cpp
    FileStoragePolicy.cpp
    FileStorageWatcher.cpp
    DatasetProvider.cpp
    AbstractTile.cpp
    TextureTile.cpp
    TileId.cpp
    TileLoader.cpp
    TileLoaderHelper.cpp
    TileCreator.cpp
    TinyWebBrowser.cpp
    #jsonparser.cpp
    VectorComposer.cpp
    VectorMap.cpp
    FileLoader.cpp
    FileManager.cpp
    FileViewModel.cpp
    GpxFileViewItem.cpp
    KmlFileViewItem.cpp
    gps/PositionTracking.cpp
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
    
    AbstractDataPlugin.cpp
    AbstractDataPluginModel.cpp
    AbstractDataPluginItem.cpp
    AbstractWorkerThread.cpp
    
    PluginInterface.cpp
    NetworkPluginInterface.cpp
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

    MarbleRunnerManager.cpp
    runners/MarbleAbstractRunner.cpp
    runners/LatLonRunner.cpp
    runners/OnfRunner.cpp
)

set (marblewidget_UI
    MarbleControlBox.ui
    MarbleNavigator.ui
    MarbleViewSettingsWidget.ui
    MarbleNavigationSettingsWidget.ui
    MarbleCacheSettingsWidget.ui
    MarblePluginSettingsWidget.ui
    TileCreatorDialog.ui
    PlacemarkInfoDialog.ui
    MarbleAboutDialog.ui
    SunControlWidget.ui
    LatLonEdit.ui
)

# FIXME: cleaner approach of src/lib/MarbleControlBox.* vs. marble.qrc
qt4_add_resources(marblewidget_SRCS libmarble.qrc ../marble.qrc)

if (QTONLY)
  qt4_wrap_ui(marblewidget_SRCS ${marblewidget_UI})
  qt4_automoc(${marblewidget_SRCS})
else (QTONLY)
  kde4_add_ui_files(marblewidget_SRCS ${marblewidget_UI})
endif (QTONLY)


# link with release version of Qt libs
if (QTONLY)
    ADD_LIBRARY(marblewidget SHARED ${marblewidget_SRCS} ${SOURCES_UI_HDRS})
    TARGET_LINK_LIBRARIES (marblewidget ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTSCRIPT_LIBRARY} ${QT_QTMAIN_LIBRARY} ${QT_QTWEBKIT_LIBRARY} )
else (QTONLY)
    KDE4_ADD_LIBRARY(marblewidget SHARED ${marblewidget_SRCS} ${SOURCES_UI_HDRS})
    TARGET_LINK_LIBRARIES (marblewidget ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBRARY} ${KDE4_KDEUI_LIBRARY} ${KDE4_KIO_LIBRARY} ${QT_QTMAIN_LIBRARY} ${QT_QTSCRIPT_LIBRARY}  ${QT_QTWEBKIT_LIBRARY} )
endif (QTONLY)

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
    MarbleWidget.h
    MarbleMap.h
    MarbleModel.h
    MarbleControlBox.h
    MarbleNavigator.h
    MarbleDataFacade.h
    AbstractFloatItem.h
    AbstractLayer/AbstractLayerInterface.h
    MapThemeManager.h
    MarbleAboutDialog.h
    MarbleWidgetInputHandler.h
    TinyWebBrowser.h

    MapThemeSortFilterProxyModel.h
    
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
    ${geodata_data_HDRS}
    geodata/geodata_export.h
    geodata/parser/GeoDocument.h
    ExtDateTime.h
    TileCreator.h
    PluginInterface.h
    RenderPlugin.h
    RenderPluginInterface.h
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
