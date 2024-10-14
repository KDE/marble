PROJECT(marblewidget VERSION ${RELEASE_SERVICE_VERSION})
set(MARBLE_CMAKECONFIGNAME Marble)

macro_optional_find_package(Phonon4Qt6 QUIET)
marble_set_package_properties( Phonon4Qt6 PROPERTIES
    DESCRIPTION "Qt-based audio library"
    URL "https://phonon.kde.org/"
    PURPOSE "Support for playback of soundcue elements"
    TYPE OPTIONAL
)
if(Phonon4Qt6_FOUND)
    set(HAVE_PHONON TRUE)
endif()
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config-phonon.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/config-phonon.h)

if(MARBLE_NO_WEBKITWIDGETS)
    add_definitions(-DMARBLE_NO_WEBKITWIDGETS)
endif()

INCLUDE_DIRECTORIES(
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/projections
 ${CMAKE_CURRENT_SOURCE_DIR}/routing
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/data
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/graphicsitem
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/scene
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/handlers
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/handlers/kml
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/handlers/dgml
 ${CMAKE_CURRENT_SOURCE_DIR}/geodata/parser
 ${CMAKE_CURRENT_SOURCE_DIR}/graphicsview
 ${CMAKE_CURRENT_SOURCE_DIR}/layers
 ${CMAKE_CURRENT_SOURCE_DIR}/osm
 ${CMAKE_CURRENT_SOURCE_DIR}/cloudsync
 ${CMAKE_SOURCE_DIR}/src/lib/astro
 ${ZLIB_INCLUDE_DIRS}
)

INCLUDE(geodata/CMakeLists.txt)
INCLUDE(graphicsview/CMakeLists.txt)
INCLUDE(layers/CMakeLists.txt)
INCLUDE(osm/CMakeLists.txt)

set(HEADER_PREFIX MARBLE_LIB)
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${MARBLE_CMAKECONFIGNAME}ConfigVersion.cmake" VERSION "${PROJECT_VERSION}" COMPATIBILITY AnyNewerVersion)


########### next target ###############

set(marblewidget_SRCS
    ${geodata_SRCS}
    ${graphicsview_SRCS}
    ${layers_SRCS}
    ${osm_SRCS}
    blendings/Blending.cpp
    blendings/BlendingAlgorithms.cpp
    blendings/BlendingFactory.cpp
    blendings/SunLightBlending.cpp
    DownloadRegion.cpp
    DownloadRegionDialog.cpp
    LatLonBoxWidget.cpp
    MarbleWidget.cpp
    MarbleAbstractPresenter.cpp
    MarbleModel.cpp
    MarbleMap.cpp
    MarbleColors.cpp
    MapViewWidget.cpp
    CelestialSortFilterProxyModel.cpp
    FileViewWidget.cpp
    TourWidget.cpp
    FlyToEditWidget.cpp
    TourControlEditWidget.cpp
    WaitEditWidget.cpp
    SoundCueEditWidget.cpp
    RemoveItemEditWidget.cpp
    TourItemDelegate.cpp
    MapViewItemDelegate.cpp
    TourPlayback.cpp
    LegendWidget.cpp
    PlaybackItem.cpp
    PlaybackAnimatedUpdateItem.cpp
    PlaybackFlyToItem.cpp
    PlaybackSoundCueItem.cpp
    PlaybackTourControlItem.cpp
    PlaybackWaitItem.cpp
    SerialTrack.cpp
    SoundTrack.cpp
    AnimatedUpdateTrack.cpp
    CurrentLocationWidget.cpp
    MarbleNavigator.cpp
    MarbleLegendBrowser.cpp
    MarbleAboutDialog.cpp
    MarbleInputHandler.cpp
    MarbleWidgetInputHandler.cpp
    MarbleWidgetPopupMenu.cpp
    MarblePlacemarkModel.cpp
    GeoDataTreeModel.cpp
    GeoUriParser.cpp
    kdescendantsproxymodel.cpp
    BranchFilterProxyModel.cpp
    TreeViewDecoratorModel.cpp
    MarbleDebug.cpp
    Tile.cpp
    TextureTile.cpp
    TileCoordsPyramid.cpp
    TileLevelRangeWidget.cpp
    TileLoader.cpp
    QtMarbleConfigDialog.cpp
    ClipPainter.cpp
    DownloadPolicy.cpp
    DownloadQueueSet.cpp
    GeoPainter.cpp
    HttpDownloadManager.cpp
    HttpJob.cpp
    RemoteIconLoader.cpp
    LayerManager.cpp
    PluginManager.cpp
    TimeControlWidget.cpp
    AbstractFloatItem.cpp
    PopupItem.cpp
    MarbleGlobal.cpp
    MarbleDirs.cpp
    MarbleLocale.cpp
    MarblePhysics.cpp
    TileCreatorDialog.cpp
    MapThemeManager.cpp
    ViewportParams.cpp
    ViewParams.cpp
    projections/AbstractProjection.cpp
    projections/CylindricalProjection.cpp
    projections/AzimuthalProjection.cpp
    projections/SphericalProjection.cpp
    projections/EquirectProjection.cpp
    projections/MercatorProjection.cpp
    projections/GnomonicProjection.cpp
    projections/StereographicProjection.cpp
    projections/LambertAzimuthalProjection.cpp
    projections/AzimuthalEquidistantProjection.cpp
    projections/VerticalPerspectiveProjection.cpp
    VisiblePlacemark.cpp
    PlacemarkLayout.cpp
    Planet.cpp
    PlanetFactory.cpp
    Quaternion.cpp
    TextureColorizer.cpp
    TextureMapperInterface.cpp
    ScanlineTextureMapperContext.cpp
    SphericalScanlineTextureMapper.cpp
    EquirectScanlineTextureMapper.cpp
    MercatorScanlineTextureMapper.cpp
    TileScalingTextureMapper.cpp
    GenericScanlineTextureMapper.cpp
    VectorTileModel.cpp
    DiscCache.cpp
    ServerLayout.cpp
    StoragePolicy.cpp
    CacheStoragePolicy.cpp
    FileStoragePolicy.cpp
    FileStorageWatcher.cpp
    StackedTile.cpp
    TileId.cpp
    StackedTileLoader.cpp
    TileLoaderHelper.cpp
    TileCreator.cpp
    #jsonparser.cpp
    FileLoader.cpp
    FileManager.cpp
    PositionTracking.cpp
    DataMigration.cpp
    ImageF.cpp
    MovieCapture.cpp
    MovieCaptureDialog.cpp
    TourCaptureDialog.cpp
    EditPlacemarkDialog.cpp
    AddLinkDialog.cpp
    FormattedTextWidget.cpp

    AbstractDataPlugin.cpp
    AbstractDataPluginModel.cpp
    AbstractDataPluginItem.cpp
    AbstractWorkerThread.cpp

    PluginInterface.cpp
    DialogConfigurationInterface.cpp
    LayerInterface.cpp
    RenderState.cpp
    RenderPlugin.cpp
    RenderPluginInterface.cpp
    PositionProviderPlugin.cpp
    PositionProviderPluginInterface.cpp
    PlacemarkPositionProviderPlugin.cpp
    RouteSimulationPositionProviderPlugin.cpp
    MarblePluginSettingsWidget.cpp
    RenderPluginModel.cpp
    PluginAboutDialog.cpp
    PluginItemDelegate.cpp

    SunLocator.cpp
    MarbleClock.cpp
    SunControlWidget.cpp
    MergedLayerDecorator.cpp

    MathHelper.cpp

    LatLonEdit.cpp
    PlacemarkEditHeader.cpp

    MapThemeSortFilterProxyModel.cpp

    TemplateDocument.cpp

    routing/AlternativeRoutesModel.cpp
    routing/Maneuver.cpp
    routing/Route.cpp
    routing/RouteRequest.cpp
    routing/RouteSegment.cpp
    routing/RoutingModel.cpp
    routing/RoutingProfile.cpp
    routing/RoutingManager.cpp
    routing/RoutingLayer.cpp
    routing/RoutingInputWidget.cpp
    routing/RoutingWidget.cpp
    routing/RoutingProfilesWidget.cpp
    routing/RoutingProfilesModel.cpp
    routing/RoutingProfileSettingsDialog.cpp
    routing/SpeakersModel.cpp
    routing/VoiceNavigationModel.cpp
    routing/instructions/InstructionTransformation.cpp
    routing/instructions/RoutingInstruction.cpp
    routing/instructions/RoutingPoint.cpp
    routing/instructions/RoutingWaypoint.cpp
    routing/instructions/WaypointParser.cpp

    ParsingRunnerManager.cpp
    ReverseGeocodingRunnerManager.cpp
    RoutingRunnerManager.cpp
    SearchRunnerManager.cpp

    AutoNavigation.cpp

    SearchRunnerPlugin.cpp
    ReverseGeocodingRunnerPlugin.cpp
    RoutingRunnerPlugin.cpp
    ParseRunnerPlugin.cpp
    SearchRunner.cpp
    ReverseGeocodingRunner.cpp
    RoutingRunner.cpp
    ParsingRunner.cpp
    RunnerTask.cpp

    BookmarkManager.cpp
    EditBookmarkDialog.cpp
    BookmarkManagerDialog.cpp
    NewBookmarkFolderDialog.cpp

    PrintOptionsWidget.cpp
    ExternalEditorDialog.cpp
    GoToDialog.cpp

    MapWizard.cpp
    OwsServiceManager.cpp
    MapThemeDownloadDialog.cpp
    GeoGraphicsScene.cpp
    ElevationModel.cpp
    MarbleLineEdit.cpp
    SearchInputWidget.cpp
    SearchWidget.cpp
    kineticmodel.cpp
    NewstuffModel.cpp
    MarbleZip.cpp

    OsmcSymbol.cpp

    StyleBuilder.cpp

    cloudsync/CloudSyncManager.cpp
    cloudsync/RouteSyncManager.cpp
    cloudsync/OwncloudSyncBackend.cpp
    cloudsync/CloudRouteModel.cpp
    cloudsync/CloudRoutesDialog.cpp
    cloudsync/RouteItem.cpp
    cloudsync/RouteItemDelegate.cpp
    cloudsync/BookmarkSyncManager.cpp
    cloudsync/MergeItem.cpp
    cloudsync/ConflictDialog.cpp

    blendings/Blending.h
    blendings/BlendingAlgorithms.h
    blendings/BlendingFactory.h
    blendings/SunLightBlending.h
    DownloadRegion.h
    DownloadRegionDialog.h
    LatLonBoxWidget.h
    MarbleWidget.h
    MarbleAbstractPresenter.h
    MarbleModel.h
    MarbleMap.h
    MarbleColors.h
    MapViewWidget.h
    CelestialSortFilterProxyModel.h
    FileViewWidget.h
    TourWidget.h
    FlyToEditWidget.h
    TourControlEditWidget.h
    WaitEditWidget.h
    SoundCueEditWidget.h
    RemoveItemEditWidget.h
    TourItemDelegate.h
    MapViewItemDelegate.h
    TourPlayback.h
    LegendWidget.h
    PlaybackItem.h
    PlaybackAnimatedUpdateItem.h
    PlaybackFlyToItem.h
    PlaybackSoundCueItem.h
    PlaybackTourControlItem.h
    PlaybackWaitItem.h
    SerialTrack.h
    SoundTrack.h
    AnimatedUpdateTrack.h
    CurrentLocationWidget.h
    MarbleNavigator.h
    MarbleLegendBrowser.h
    MarbleAboutDialog.h
    MarbleInputHandler.h
    MarbleWidgetInputHandler.h
    MarbleWidgetPopupMenu.h
    MarblePlacemarkModel.h
    GeoDataTreeModel.h
    GeoUriParser.h
    kdescendantsproxymodel.h
    BranchFilterProxyModel.h
    TreeViewDecoratorModel.h
    MarbleDebug.h
    Tile.h
    TextureTile.h
    TileCoordsPyramid.h
    TileLevelRangeWidget.h
    TileLoader.h
    QtMarbleConfigDialog.h
    ClipPainter.h
    DownloadPolicy.h
    DownloadQueueSet.h
    GeoPainter.h
    HttpDownloadManager.h
    HttpJob.h
    RemoteIconLoader.h
    LayerManager.h
    PluginManager.h
    TimeControlWidget.h
    AbstractFloatItem.h
    PopupItem.h
    MarbleGlobal.h
    MarbleDirs.h
    MarbleLocale.h
    MarblePhysics.h
    TileCreatorDialog.h
    MapThemeManager.h
    ViewportParams.h
    ViewParams.h
    projections/AbstractProjection.h
    projections/CylindricalProjection.h
    projections/AzimuthalProjection.h
    projections/SphericalProjection.h
    projections/EquirectProjection.h
    projections/MercatorProjection.h
    projections/GnomonicProjection.h
    projections/StereographicProjection.h
    projections/LambertAzimuthalProjection.h
    projections/AzimuthalEquidistantProjection.h
    projections/VerticalPerspectiveProjection.h
    VisiblePlacemark.h
    PlacemarkLayout.h
    Planet.h
    PlanetFactory.h
    Quaternion.h
    TextureColorizer.h
    TextureMapperInterface.h
    ScanlineTextureMapperContext.h
    SphericalScanlineTextureMapper.h
    EquirectScanlineTextureMapper.h
    MercatorScanlineTextureMapper.h
    TileScalingTextureMapper.h
    GenericScanlineTextureMapper.h
    VectorTileModel.h
    DiscCache.h
    ServerLayout.h
    StoragePolicy.h
    CacheStoragePolicy.h
    FileStoragePolicy.h
    FileStorageWatcher.h
    StackedTile.h
    TileId.h
    StackedTileLoader.h
    TileLoaderHelper.h
    TileCreator.h
    #jsonparser.h
    FileLoader.h
    FileManager.h
    PositionTracking.h
    DataMigration.h
    ImageF.h
    MovieCapture.h
    MovieCaptureDialog.h
    TourCaptureDialog.h
    EditPlacemarkDialog.h
    AddLinkDialog.h
    FormattedTextWidget.h

    AbstractDataPlugin.h
    AbstractDataPluginModel.h
    AbstractDataPluginItem.h
    AbstractWorkerThread.h

    PluginInterface.h
    DialogConfigurationInterface.h
    LayerInterface.h
    RenderState.h
    RenderPlugin.h
    RenderPluginInterface.h
    PositionProviderPlugin.h
    PositionProviderPluginInterface.h
    PlacemarkPositionProviderPlugin.h
    RouteSimulationPositionProviderPlugin.h
    MarblePluginSettingsWidget.h
    RenderPluginModel.h
    PluginAboutDialog.h
    PluginItemDelegate.h

    SunLocator.h
    MarbleClock.h
    SunControlWidget.h
    MergedLayerDecorator.h

    MathHelper.h

    LatLonEdit.h
    PlacemarkEditHeader.h

    MapThemeSortFilterProxyModel.h

    TemplateDocument.h

    routing/AlternativeRoutesModel.h
    routing/Maneuver.h
    routing/Route.h
    routing/RouteRequest.h
    routing/RouteSegment.h
    routing/RoutingModel.h
    routing/RoutingProfile.h
    routing/RoutingManager.h
    routing/RoutingLayer.h
    routing/RoutingInputWidget.h
    routing/RoutingWidget.h
    routing/RoutingProfilesWidget.h
    routing/RoutingProfilesModel.h
    routing/RoutingProfileSettingsDialog.h
    routing/SpeakersModel.h
    routing/VoiceNavigationModel.h
    routing/instructions/InstructionTransformation.h
    routing/instructions/RoutingInstruction.h
    routing/instructions/RoutingPoint.h
    routing/instructions/RoutingWaypoint.h
    routing/instructions/WaypointParser.h

    ParsingRunnerManager.h
    ReverseGeocodingRunnerManager.h
    RoutingRunnerManager.h
    SearchRunnerManager.h

    AutoNavigation.h

    SearchRunnerPlugin.h
    ReverseGeocodingRunnerPlugin.h
    RoutingRunnerPlugin.h
    ParseRunnerPlugin.h
    SearchRunner.h
    ReverseGeocodingRunner.h
    RoutingRunner.h
    ParsingRunner.h
    RunnerTask.h

    BookmarkManager.h
    EditBookmarkDialog.h
    BookmarkManagerDialog.h
    NewBookmarkFolderDialog.h

    PrintOptionsWidget.h
    ExternalEditorDialog.h
    GoToDialog.h

    MapWizard.h
    OwsServiceManager.h
    MapThemeDownloadDialog.h
    GeoGraphicsScene.h
    ElevationModel.h
    MarbleLineEdit.h
    SearchInputWidget.h
    SearchWidget.h
    kineticmodel.h
    NewstuffModel.h

    OsmcSymbol.h

    StyleBuilder.h

    cloudsync/CloudSyncManager.h
    cloudsync/RouteSyncManager.h
    cloudsync/OwncloudSyncBackend.h
    cloudsync/CloudRouteModel.h
    cloudsync/CloudRoutesDialog.h
    cloudsync/RouteItem.h
    cloudsync/RouteItemDelegate.h
    cloudsync/BookmarkSyncManager.h
    cloudsync/MergeItem.h
    cloudsync/ConflictDialog.h

)


if(MARBLE_NO_WEBKITWIDGETS)
    LIST(APPEND marblewidget_SRCS
        NullMarbleWebView.cpp
        NullTinyWebBrowser.cpp
)
else()
    LIST(APPEND marblewidget_SRCS
        MarbleWebView.cpp
        TinyWebBrowser.cpp
        MarbleWebView.h
        TinyWebBrowser.h
)
endif()

if(NOT MARBLE_NO_DBUS)
    LIST(APPEND marblewidget_SRCS
        MarbleDBusInterface.cpp
        MarbleDBusInterface.h
)
endif()

set(marblewidget_UI
    ${osm_UIS}
    LatLonBoxWidget.ui
    MapViewWidget.ui
    CurrentLocationWidget.ui
    FileViewWidget.ui
    TourWidget.ui
    MarbleNavigator.ui
    MarbleCacheSettingsWidget.ui
    MarbleViewSettingsWidget.ui
    MarbleNavigationSettingsWidget.ui
    TimeControlWidget.ui
    MarbleTimeSettingsWidget.ui
    MarblePluginSettingsWidget.ui
    TileCreatorDialog.ui
    TileLevelRangeWidget.ui
    MarbleAboutDialog.ui
    SunControlWidget.ui
    LatLonEdit.ui
    PlacemarkEditHeader.ui
    routing/RoutingWidget.ui
    DataMigrationWidget.ui
    EditBookmarkDialog.ui
    BookmarkManagerDialog.ui
    NewBookmarkFolderDialog.ui
    PrintOptions.ui
    ExternalEditor.ui
    GoToDialog.ui
    routing/RoutingSettingsWidget.ui
    routing/RoutingProfileSettingsDialog.ui
    MapWizard.ui
    MapThemeDownloadDialog.ui
    cloudsync/CloudRoutesDialog.ui
    MarbleCloudSyncSettingsWidget.ui
    MovieCaptureDialog.ui
    TourCaptureDialog.ui
    EditPlacemarkDialog.ui
    AddLinkDialog.ui
    FormattedTextWidget.ui
    ElevationWidget.ui
)
if(MARBLE_NO_WEBKITWIDGETS)
    LIST(APPEND marblewidget_UI
        NullLegendWidget.ui
        NullWebPopupWidget.ui
)
else()
    LIST(APPEND marblewidget_UI
        LegendWidget.ui
        WebPopupWidget.ui
)
endif()

# FIXME: cleaner approach of src/lib/marblwidget/MarbleControlBox.* vs. marble.qrc
qt_add_resources(marblewidget_SRCS libmarble.qrc ../../apps/marble-ui/marble.qrc)

qt_wrap_ui(marblewidget_SRCS ${marblewidget_UI})

add_library(marblewidget ${marblewidget_SRCS} ${SOURCES_UI_HDRS})

generate_export_header(marblewidget BASE_NAME marble)

# Logging for the Marble library
ecm_qt_export_logging_category(
    IDENTIFIER "MARBLE_LIB"
    CATEGORY_NAME "marble_lib"
    EXPORT marble
    DESCRIPTION "Marble (library)"
)
add_definitions("-DLOGGING_IDENTIFIER=MARBLE_LIB")

# link with libastro
target_link_libraries(marblewidget PRIVATE astro)

# link against zlib
target_link_libraries(marblewidget PRIVATE ${ZLIB_LIBRARIES})

# link with release version of Qt libs
target_link_libraries(marblewidget
    PUBLIC
        Qt6::Xml
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::Core5Compat
    PRIVATE
        Qt6::Core
        Qt6::Svg
        Qt6::PrintSupport
        Qt6::Concurrent
)
if(NOT MARBLE_NO_WEBKITWIDGETS)
    target_link_libraries(marblewidget
        PUBLIC
            Qt6::WebChannel
            Qt6::WebEngineWidgets
)
endif()
if(NOT MARBLE_NO_DBUS)
    target_link_libraries(marblewidget
        PRIVATE
            Qt6::DBus
)
endif()
if(Phonon4Qt6_FOUND)
    target_link_libraries(marblewidget
        PRIVATE
            Phonon::phonon4qt6
)
endif()

if(APPLE)
  # TODO: PUBLIC or can be PRIVATE?
  #defined in top level makefile
  target_link_libraries(marblewidget PUBLIC ${MAC_EXTRA_LIBS})
endif(APPLE)

# TODO: still supported?
if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  target_link_libraries(marblewidget PUBLIC m)
endif(CMAKE_SYSTEM_NAME MATCHES "SunOS")

if(WIN32)
  # TODO: PUBLIC or can be PRIVATE?
  target_link_libraries(marblewidget PUBLIC ws2_32 imm32 winmm)
endif(WIN32)

set_target_properties(marblewidget  PROPERTIES
    EXPORT_NAME "Marble"
    OUTPUT_NAME marblewidget-qt6
    COMPILE_FLAGS "-DKML_LAZY_IMP"
)

if(NOT ANDROID)
  set_target_properties(marblewidget  PROPERTIES
    VERSION ${MARBLE_VERSION}
    SOVERSION ${MARBLE_SOVERSION}
)
endif()

target_include_directories(marblewidget
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

# choose the correct target install library path
if(WIN32)
  install(TARGETS marblewidget EXPORT MarbleTargets RUNTIME DESTINATION . ARCHIVE DESTINATION lib)
elseif(APPLE)
  install(TARGETS marblewidget LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/Marble.app/Contents/MacOS/lib)
else()
  install(TARGETS marblewidget
    EXPORT MarbleTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()


########### install headers & cmake config files ###############

if(APPLE)
    # TODO: find how to install & deploy library, headers and config files as bundle for development
    return()
endif()

if(MARBLE_NO_WEBKITWIDGETS)
    set(marble_WebKit
        NullMarbleWebView.h
        NullTinyWebBrowser.h
)
else()
    set(marble_WebKit
        MarbleWebView.h
        TinyWebBrowser.h
)
endif()

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/marble_version.h
    ${CMAKE_CURRENT_BINARY_DIR}/marble_export.h
    ${graphicsview_HDRS}
    ${marble_WebKit}
    AutoNavigation.h
    BookmarkManager.h
    DownloadRegion.h
    DownloadRegionDialog.h
    FileManager.h
    LatLonBoxWidget.h
    MarbleWidget.h
    MarbleMap.h
    MarbleModel.h
    MapViewWidget.h
    CelestialSortFilterProxyModel.h
    LegendWidget.h
    FileViewWidget.h
    TourWidget.h
    FlyToEditWidget.h
    TourControlEditWidget.h
    WaitEditWidget.h
    SoundCueEditWidget.h
    RemoveItemEditWidget.h
    TourItemDelegate.h
    MapViewItemDelegate.h
    TourPlayback.h
    CurrentLocationWidget.h
    MarbleNavigator.h
    AbstractFloatItem.h
    MapThemeManager.h
    MarbleAboutDialog.h
    MarbleInputHandler.h
    MarbleWidgetInputHandler.h
    MarbleWidgetPopupMenu.h
    TileId.h
    TileCoordsPyramid.h
    TileLevelRangeWidget.h
    QtMarbleConfigDialog.h
    MarbleMath.h
    MarbleColors.h
    MarbleGlobal.h
    MarbleLocale.h
    MarbleDebug.h
    MarbleDirs.h
    GeoPainter.h
    HttpDownloadManager.h
    TileCreatorDialog.h
    ViewportParams.h
    projections/AbstractProjection.h
    PositionTracking.h
    Quaternion.h
    SunLocator.h
    ClipPainter.h
    GeoGraphicsScene.h
    GeoDataTreeModel.h
    geodata/data/GeoDataAbstractView.h
    geodata/data/GeoDataAccuracy.h
    geodata/data/GeoDataBalloonStyle.h
    geodata/data/GeoDataColorStyle.h
    geodata/data/GeoDataContainer.h
    geodata/data/GeoDataCoordinates.h
    geodata/data/GeoDataDocument.h
    geodata/data/GeoDataFeature.h
    geodata/data/GeoDataFolder.h
    geodata/data/GeoDataGeometry.h
    geodata/data/GeoDataGroundOverlay.h
    geodata/data/GeoDataHotSpot.h
    geodata/data/GeoDataIconStyle.h
    geodata/data/GeoDataItemIcon.h
    geodata/data/GeoDataLabelStyle.h
    geodata/data/GeoDataLatLonAltBox.h
    geodata/data/GeoDataLatLonBox.h
    geodata/data/GeoDataLatLonQuad.h
    geodata/data/GeoDataLinearRing.h
    geodata/data/GeoDataLineString.h
    geodata/data/GeoDataLineStyle.h
    geodata/data/GeoDataListStyle.h
    geodata/data/GeoDataLod.h
    geodata/data/GeoDataLookAt.h
    geodata/data/GeoDataOverlay.h
    geodata/data/GeoDataMultiGeometry.h
    geodata/data/GeoDataObject.h
    geodata/data/GeoDataPlacemark.h
    geodata/data/GeoDataPoint.h
    geodata/data/GeoDataPolygon.h
    geodata/data/GeoDataPolyStyle.h
    geodata/data/GeoDataRegion.h
    geodata/data/GeoDataRelation.h
    geodata/data/GeoDataSnippet.h
    geodata/data/GeoDataStyle.h
    geodata/data/GeoDataStyleMap.h
    geodata/data/GeoDataStyleSelector.h
    geodata/data/GeoDataTrack.h
    geodata/data/GeoDataTimeSpan.h
    geodata/data/GeoDataTimeStamp.h
    geodata/data/GeoDataTimePrimitive.h
    geodata/data/Serializable.h
    geodata/geodata_export.h
    geodata/parser/GeoDocument.h
    geodata/parser/GeoDataTypes.h
    geodata/writer/GeoWriter.h
    routing/RoutingWidget.h
    routing/RoutingManager.h
    TileCreator.h
    PluginManager.h
    PluginInterface.h
    DialogConfigurationInterface.h
    PositionProviderPlugin.h
    PositionProviderPluginInterface.h
    RenderPlugin.h
    RenderPluginInterface.h
    ParsingRunnerManager.h
    ReverseGeocodingRunnerManager.h
    RoutingRunnerManager.h
    SearchRunnerManager.h
    ParsingRunner.h
    SearchRunner.h
    ReverseGeocodingRunner.h
    RoutingRunner.h
    SearchRunnerPlugin.h
    ReverseGeocodingRunnerPlugin.h
    RoutingRunnerPlugin.h
    ParseRunnerPlugin.h
    LayerInterface.h
    RenderState.h
    PluginAboutDialog.h
    Planet.h
    PlanetFactory.h
    EditPlacemarkDialog.h
    AddLinkDialog.h
    FormattedTextWidget.h

    AbstractDataPlugin.h
    AbstractDataPluginModel.h
    AbstractDataPluginItem.h
    AbstractWorkerThread.h

    LatLonEdit.h
    PlacemarkEditHeader.h
    MapWizard.h
    MapThemeDownloadDialog.h
    ElevationModel.h

    routing/AlternativeRoutesModel.h
    routing/Route.h
    routing/Maneuver.h
    routing/RouteRequest.h
    routing/RouteSegment.h
    routing/RoutingManager.h
    routing/RoutingModel.h
    routing/RoutingProfile.h

    OsmcSymbol.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/marble
)


########### CMake Config files ###############

include(CMakePackageConfigHelpers)

set(MARBLE_INSTALL_CMAKECONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${MARBLE_CMAKECONFIGNAME})

configure_package_config_file(MarbleConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${MARBLE_CMAKECONFIGNAME}Config.cmake
    INSTALL_DESTINATION ${MARBLE_INSTALL_CMAKECONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${MARBLE_CMAKECONFIGNAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${MARBLE_CMAKECONFIGNAME}ConfigVersion.cmake

    DESTINATION ${MARBLE_INSTALL_CMAKECONFIGDIR}
)

install(
    EXPORT MarbleTargets
    FILE ${MARBLE_CMAKECONFIGNAME}Targets.cmake
    DESTINATION ${MARBLE_INSTALL_CMAKECONFIGDIR}
)

########### Qt Pri file ###############

# Where the pri file is installed is defined by the following rules, with decreasing priority:
# * MARBLE_PRI_INSTALL_USE_QT_SYS_PATHS=TRUE:
#   pri file will be installed with the dirs of the Qt installation used
# * MARBLE_PRI_INSTALL_DIR=some-relative-or-absolute-path
#   if set will be used as given
# * installation prefix = Qt installation prefix:
#   pri file will be installed with the dirs of the Qt installation used
# * otherwise:
#   pri file will be installed in mkspecs/modules in the installation prefix

get_target_property(PRI_LIB_NAME marblewidget OUTPUT_NAME)

set(PRI_ABS_INCLUDE_INSTALL_DIR "${INCLUDE_INSTALL_DIR}")
if(NOT IS_ABSOLUTE "${PRI_ABS_INCLUDE_INSTALL_DIR}")
    set(PRI_ABS_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${PRI_ABS_INCLUDE_INSTALL_DIR}")
endif()

set(PRI_ABS_LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
if(NOT IS_ABSOLUTE "${PRI_ABS_LIB_INSTALL_DIR}")
    set(PRI_ABS_LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${PRI_ABS_LIB_INSTALL_DIR}")
endif()

set(PRI_DEPS "xml gui widgets network")
if(NOT MARBLE_NO_WEBKITWIDGETS)
    set(PRI_DEPS "${PRI_DEPS} webenginewidgets")
endif()

file(GENERATE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qt_Marble.pri
    CONTENT
"QT.Marble.VERSION = ${MARBLE_LIB_VERSION}
QT.Marble.MAJOR_VERSION = ${MARBLE_LIB_VERSION_MAJOR}
QT.Marble.MINOR_VERSION = ${MARBLE_LIB_VERSION_MINOR}
QT.Marble.PATCH_VERSION = ${MARBLE_LIB_VERSION_PATCH}
QT.Marble.name = ${PRI_LIB_NAME}
QT.Marble.defines =
QT.Marble.includes = ${PRI_ABS_INCLUDE_INSTALL_DIR}
QT.Marble.private_includes =
QT.Marble.libs = ${PRI_ABS_LIB_INSTALL_DIR}
QT.Marble.depends = ${PRI_DEPS}
"
)

# detect which folder the pri file should be installed into
get_target_property(QMAKE_EXECUTABLE Qt6::qmake LOCATION)

function(query_qmake result_variable qt_variable)
    execute_process(
        COMMAND ${QMAKE_EXECUTABLE} -query "${qt_variable}"
        RESULT_VARIABLE return_code
        OUTPUT_VARIABLE output
)
    if(return_code EQUAL 0)
        string(STRIP "${output}" output)
        file(TO_CMAKE_PATH "${output}" output_path)
        set(${result_variable} "${output_path}" PARENT_SCOPE)
    else()
        message(WARNING "Failed call: ${QMAKE_EXECUTABLE} -query \"${qt_variable}\"")
        message(FATAL_ERROR "QMake call failed: ${return_code}")
    endif()
endfunction()

# if going to be installed into same prefix as Qt, then also use same folder
set(IS_SHARED_INSTALL_PREFIX FALSE)
if(NOT DEFINED MARBLE_PRI_INSTALL_USE_QT_SYS_PATHS)
    query_qmake(qt_install_prefix_dir QT_INSTALL_PREFIX)
    if(qt_install_prefix_dir STREQUAL "${CMAKE_INSTALL_PREFIX}")
        set(IS_SHARED_INSTALL_PREFIX TRUE)
    endif()
endif()

if(MARBLE_PRI_INSTALL_USE_QT_SYS_PATHS OR IS_SHARED_INSTALL_PREFIX)
    query_qmake(qt_host_data_dir QT_HOST_DATA)
    set(PRI_INSTALL_DIR ${qt_host_data_dir}/mkspecs/modules)
else()
    set(PRI_INSTALL_DIR mkspecs/modules)
endif()
set(MARBLE_PRI_INSTALL_DIR "${PRI_INSTALL_DIR}" CACHE PATH "The directory where the Marble pri file will be installed to.")

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/qt_Marble.pri
    DESTINATION  ${MARBLE_PRI_INSTALL_DIR}
)
