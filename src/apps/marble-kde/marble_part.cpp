//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig  <tokoe@kde.org>
// Copyright 2008      Inge Wallin    <inge@lysator.liu.se>
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2010      Harshit Jain   <hjain.itbhu@gmail.com>
//

// Own
#include "marble_part.h"

// Qt
#include <QDir>
#include <QPointer>
#include <QStringList>
#include <QClipboard>
#include <QLabel>
#include <QFontMetrics>
#include <QPrinter>
#include <QPrintDialog>
#include <QProgressBar>
#include <QStandardItemModel>
#include <QNetworkProxy>
#include <QDomDocument>
#include <QDomNodeList>
#include <QFileDialog>
#include <QMenu>
#include <QStatusBar>

// KF
#include <kaboutdata.h>
#include <kactioncollection.h>
#include <kconfigdialog.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <kpluginfactory.h>
#include <kparts/statusbarextension.h>
#include <kstandardaction.h>
#include <ktoggleaction.h>
#include <ktogglefullscreenaction.h>
#include <kns3/knewstuffaction.h>
#include <kns3/uploaddialog.h>

// Marble library classes
#include "AbstractFloatItem.h"
#include "EditBookmarkDialog.h"
#include "BookmarkManager.h"
#include "BookmarkManagerDialog.h"
#include "CurrentLocationWidget.h"
#include "DownloadRegionDialog.h"
#include "GeoDataCoordinates.h"
#include "GeoDataFolder.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLookAt.h"
#include "GeoDataPlacemark.h"
#include "HttpDownloadManager.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "MarblePluginSettingsWidget.h"
#include "MapWizard.h"
#include "NewBookmarkFolderDialog.h"
#include "RenderPluginModel.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingProfilesModel.h"
#include "routing/RoutingProfilesWidget.h"
#include "routing/RouteRequest.h"
#include "SunControlWidget.h"
#include "TimeControlWidget.h"
#include "TileCoordsPyramid.h"
#include "ViewportParams.h"
#include "MarbleClock.h"
#include "ParseRunnerPlugin.h"
#include "PositionTracking.h"
#include "PositionProviderPlugin.h"
#include "PluginManager.h"
#include "SearchInputWidget.h"
#include "MarbleWidgetInputHandler.h"
#include "Planet.h"
#include "MapThemeDownloadDialog.h"
#include "cloudsync/BookmarkSyncManager.h"
#include "MovieCaptureDialog.h"
#include "cloudsync/RouteSyncManager.h"
#include "cloudsync/CloudSyncManager.h"

// Marble non-library classes
#include "ControlView.h"
#include "settings.h"

using namespace Marble;

#include "ui_MarbleCacheSettingsWidget.h"
#include "ui_MarbleViewSettingsWidget.h"
#include "ui_MarbleNavigationSettingsWidget.h"
#include "ui_MarbleTimeSettingsWidget.h"

namespace Marble
{

namespace
{
    const char POSITION_STRING[] = I18N_NOOP( "Position: %1" );
    const char DISTANCE_STRING[] = I18N_NOOP( "Altitude: %1" );
    const char TILEZOOMLEVEL_STRING[] = I18N_NOOP( "Tile Zoom Level: %1" );
    const char DATETIME_STRING[] = I18N_NOOP( "Time: %1" );
}

K_PLUGIN_FACTORY(MarblePartFactory, registerPlugin<MarblePart>();)
K_EXPORT_PLUGIN(MarblePartFactory("marble"))

MarblePart::MarblePart( QWidget *parentWidget, QObject *parent, const QVariantList &arguments )
  : KParts::ReadOnlyPart( parent ),
    m_sunControlDialog( 0 ),
    m_timeControlDialog( 0 ),
    m_downloadRegionDialog( 0 ),
    m_movieCaptureDialog( 0 ),
    m_externalMapEditorAction( 0 ),
    m_recordMovieAction( 0 ),
    m_stopRecordingAction( 0 ),
    m_recentFilesAction( 0 ),
    m_configDialog( 0 ),
    m_position( QCoreApplication::translate( "Marble", NOT_AVAILABLE ) ),
    m_tileZoomLevel( QCoreApplication::translate( "Marble", NOT_AVAILABLE ) ),
    m_positionLabel( 0 ),
    m_distanceLabel( 0 )
{
    // only set marble data path when a path was given
    if ( arguments.count() != 0 && !arguments.first().toString().isEmpty() )
        MarbleDirs::setMarbleDataPath( arguments.first().toString() );

    // Setting measure system to provide nice standards for all unit questions.
    // This has to happen before any initialization so plugins (for example) can
    // use it during initialization.
    MarbleLocale *marbleLocale = MarbleGlobal::getInstance()->locale();
    // KF5 TODO: Read Connecting Calls to Catalogs" in ki18n programmer's guide
    //           and enable translations
    //     KLocale *kLocale = KGlobal::locale();
//     if ( kLocale->measureSystem() == KLocale::Metric ) {
//         marbleLocale->setMeasurementSystem( QLocale::MetricSystem );
//     }
//     else {
//         marbleLocale->setMeasurementSystem( QLocale::ImperialSystem );
//     }

    marbleLocale->setMeasurementSystem( MarbleLocale::ImperialSystem );
    migrateNewstuffConfigFiles();

    m_externalEditorMapping[0] = "";
    m_externalEditorMapping[1] = "potlatch";
    m_externalEditorMapping[2] = "josm";
    m_externalEditorMapping[3] = "merkaartor";

    m_controlView = new ControlView( parentWidget );

    setWidget( m_controlView );

    setupActions();

    setXMLFile( "marble_part.rc" );

    m_statusBarExtension = new KParts::StatusBarExtension( this );
    m_statusBarExtension->statusBar()->setUpdatesEnabled( false );

    // Load bookmark file. If it does not exist, a default one will be used.
    m_controlView->marbleModel()->bookmarkManager()->loadFile( "bookmarks/bookmarks.kml" );

    initializeCustomTimezone();

    setupStatusBar();
    readSettings();
    m_statusBarExtension->statusBar()->setUpdatesEnabled( true );

    // Show startup location
    switch ( MarbleSettings::onStartup() ) {
    case LastLocationVisited: {
            GeoDataLookAt target;
            target.setLongitude( MarbleSettings::quitLongitude() );
            target.setLatitude( MarbleSettings::quitLatitude() );
            target.setRange( MarbleSettings::quitRange() );
            m_controlView->marbleWidget()->flyTo( target, Instant );
        }
        break;
    case ShowHomeLocation:
        m_controlView->marbleWidget()->goHome( Instant );
        break;
    }

    connect( m_controlView, SIGNAL(showUploadDialog()), this, SLOT(showUploadNewStuffDialog()) );
    connect( m_controlView, SIGNAL(showMapWizard()), this, SLOT(showMapWizard()) );
    connect( m_controlView, SIGNAL(mapThemeDeleted()), this, SLOT(fallBackToDefaultTheme()) );
}

MarblePart::~MarblePart()
{
    writeSettings();

    // Check whether this delete is really needed.
    delete m_configDialog;
}

ControlView* MarblePart::controlView() const
{
    return m_controlView;
}

KAboutData *MarblePart::createAboutData()
{
    return new KAboutData( QString( I18N_NOOP( "marble_part" ) ),
                           QString( "Marble" ),
                           ControlView::applicationVersion(),
                           i18n( "A Virtual Globe" ),
                           KAboutLicense::LGPL_V2 );
}

bool MarblePart::openUrl( const QUrl &url )
{
    QFileInfo fileInfo( url.toLocalFile() );
    if ( fileInfo.isReadable() ) {
        m_controlView->marbleModel()->addGeoDataFile( url.toLocalFile() );
        m_recentFilesAction->addUrl( url );
        return true;
    }

    KMessageBox::error( widget(),
        i18n( "Sorry, unable to open '%1'. The file is not accessible." ).arg( fileInfo.fileName() ),
        i18n( "File not accessible" ) );
    return false;
}

bool MarblePart::openFile()
{
    const PluginManager *const pluginManager = m_controlView->marbleModel()->pluginManager();

    QStringList allFileExtensions;
    QStringList filters;
    foreach ( const ParseRunnerPlugin *plugin, pluginManager->parsingRunnerPlugins() ) {
        if (plugin->nameId() == QLatin1String("Cache"))
            continue;

        const QStringList fileExtensions = plugin->fileExtensions().replaceInStrings( QRegExp( "^" ), "*." );
        const QString filter = plugin->fileFormatDescription() + QLatin1String(" (") + fileExtensions.join(QLatin1Char(' ')) + QLatin1Char(')');
        filters << filter;
        allFileExtensions << fileExtensions;
    }

    allFileExtensions.sort();  // sort since file extensions are visible under Windows
    const QString allFileTypes = i18n("All Supported Files") + QLatin1String(" (") + allFileExtensions.join(QLatin1Char(' ')) + QLatin1Char(')');

    filters.sort();
    filters.prepend( allFileTypes );
    const QString filter = filters.join( ";;" );

    QStringList fileNames = QFileDialog::getOpenFileNames( widget(), i18n("Open File"),
                                                           m_lastFileOpenPath, filter );

    if ( !fileNames.isEmpty() ) {
        const QString firstFile = fileNames.first();
        m_lastFileOpenPath = QFileInfo( firstFile ).absolutePath();
    }

    foreach( const QString &fileName, fileNames ) {
        openUrl( QUrl::fromLocalFile(fileName) );
    }

    return true;
}

void MarblePart::exportMapScreenShot()
{
    QString  fileName = QFileDialog::getSaveFileName( widget(), i18n("Export Map"), QDir::homePath(),
                                                      i18n( "Images *.jpg *.png" ) );

    if ( !fileName.isEmpty() ) {
        // Take the case into account where no file format is indicated
        const char * format = 0;
        if ( !fileName.endsWith(QLatin1String( "png" ), Qt::CaseInsensitive)
           && !fileName.endsWith(QLatin1String( "jpg" ), Qt::CaseInsensitive) )
        {
            format = "JPG";
        }

        QPixmap mapPixmap = m_controlView->mapScreenShot();
        bool  success = mapPixmap.save( fileName, format );
        if ( !success ) {
            KMessageBox::error( widget(), i18nc( "Application name", "Marble" ),
                                i18n( "An error occurred while trying to save the file.\n" ),
                                KMessageBox::Notify );
        }
    }
}

void MarblePart::setShowBookmarks( bool show )
{
    m_controlView->marbleModel()->bookmarkManager()->setShowBookmarks( show );

    m_toggleBookmarkDisplayAction->setChecked( show ); // Sync state with the GUI
}

void MarblePart::setShowClouds( bool isChecked )
{
    m_controlView->marbleWidget()->setShowClouds( isChecked );

    m_showCloudsAction->setChecked( isChecked ); // Sync state with the GUI
}

void MarblePart::showPositionLabel( bool isChecked )
{
    m_positionLabel->setVisible( isChecked );
}

void MarblePart::showAltitudeLabel( bool isChecked )
{
    m_distanceLabel->setVisible( isChecked );
}

void MarblePart::showTileZoomLevelLabel( bool isChecked )
{
    m_tileZoomLevelLabel->setVisible( isChecked );
}

void MarblePart::showDateTimeLabel( bool isChecked )
{
    m_clockLabel->setVisible( isChecked );
}

void MarblePart::showDownloadProgressBar( bool isChecked )
{
    MarbleSettings::setShowDownloadProgressBar( isChecked );
    // Change visibility only if there is a download happening
    m_downloadProgressBar->setVisible( isChecked && m_downloadProgressBar->value() >= 0 );
}

void MarblePart::showFullScreen( bool isChecked )
{
    if ( QApplication::activeWindow() )
        KToggleFullScreenAction::setFullScreen( QApplication::activeWindow(), isChecked );

    m_fullScreenAct->setChecked( isChecked ); // Sync state with the GUI
}

void MarblePart::showStatusBar( bool isChecked )
{
    if ( !m_statusBarExtension->statusBar() )
        return;

    m_statusBarExtension->statusBar()->setVisible( isChecked );
}

void MarblePart::controlSun()
{
    if ( !m_sunControlDialog ) {
        m_sunControlDialog = new SunControlWidget( m_controlView->marbleWidget(), m_controlView );
        connect( m_sunControlDialog, SIGNAL(showSun(bool)),
                 this,               SLOT (showSun(bool)) );
        connect( m_sunControlDialog, SIGNAL(showSun(bool)),
                 m_showShadow,               SLOT (setChecked(bool)) );
        connect( m_sunControlDialog,    SIGNAL(isLockedToSubSolarPoint(bool)),
                 m_lockToSubSolarPoint, SLOT (setChecked(bool)) );
        connect( m_sunControlDialog,         SIGNAL(isSubSolarPointIconVisible(bool)),
                 m_setSubSolarPointIconVisible, SLOT (setChecked(bool)) );
    }

    m_sunControlDialog->show();
    m_sunControlDialog->raise();
    m_sunControlDialog->activateWindow();
}

void MarblePart::controlTime()
{
    if ( !m_timeControlDialog )
    {
        m_timeControlDialog = new TimeControlWidget( m_controlView->marbleModel()->clock() );
    }
    m_timeControlDialog->show();
    m_timeControlDialog->raise();
    m_timeControlDialog->activateWindow();
}


void MarblePart::showSun( bool active )
{
    m_controlView->marbleWidget()->setShowSunShading( active );
    m_sunControlDialog->setSunShading( active );
}

void MarblePart::lockToSubSolarPoint( bool lock )
{
    m_controlView->marbleWidget()->setLockToSubSolarPoint( lock );
}

void MarblePart::setSubSolarPointIconVisible( bool show )
{
    m_controlView->marbleWidget()->setSubSolarPointIconVisible( show );
}

void MarblePart::workOffline( bool offline )
{
    m_controlView->setWorkOffline( offline );
    m_newStuffAction->setEnabled( !offline );
    m_downloadRegionAction->setEnabled( !offline );
}

void MarblePart::copyMap()
{
    QPixmap      mapPixmap = m_controlView->mapScreenShot();
    QClipboard  *clipboard = QApplication::clipboard();

    clipboard->setPixmap( mapPixmap );
}

void MarblePart::copyCoordinates()
{
    qreal lon = m_controlView->marbleWidget()->centerLongitude();
    qreal lat = m_controlView->marbleWidget()->centerLatitude();

    QString  positionString = GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Degree ).toString();
    QClipboard  *clipboard = QApplication::clipboard();

    clipboard->setText( positionString );
}

void MarblePart::readSettings()
{
    qDebug() << "Start: MarblePart::readSettings()";

    // Set home position
    m_controlView->marbleModel()->setHome( MarbleSettings::homeLongitude(),
                                           MarbleSettings::homeLatitude(),
                                           MarbleSettings::homeZoom() );

    // Map theme and projection
    QString mapTheme = MarbleSettings::mapTheme();
    if ( mapTheme.isEmpty() ) {
        mapTheme = m_controlView->defaultMapThemeId();
    }
    m_controlView->marbleWidget()->setMapThemeId( mapTheme );
    m_controlView->marbleWidget()->setProjection( (Projection) MarbleSettings::projection() );

    m_controlView->marbleWidget()->setShowClouds( MarbleSettings::showClouds() );
    m_showCloudsAction->setChecked( MarbleSettings::showClouds() );

    workOffline( MarbleSettings::workOffline() );
    m_workOfflineAction->setChecked( MarbleSettings::workOffline() );

    m_lockFloatItemsAct->setChecked(MarbleSettings::lockFloatItemPositions());
    lockFloatItemPosition(MarbleSettings::lockFloatItemPositions());

    setShowBookmarks( MarbleSettings::showBookmarks() );

    // Sun
    m_controlView->marbleWidget()->setShowSunShading( MarbleSettings::showSun() );
    m_showShadow->setChecked( MarbleSettings::showSun() );
    m_controlView->marbleWidget()->setShowCityLights( MarbleSettings::showCitylights() );
    m_controlView->marbleWidget()->setSubSolarPointIconVisible( MarbleSettings::subSolarPointIconVisible() );
    m_controlView->marbleWidget()->setLockToSubSolarPoint( MarbleSettings::lockToSubSolarPoint() );
    m_setSubSolarPointIconVisible->setChecked( MarbleSettings::subSolarPointIconVisible() );
    m_lockToSubSolarPoint->setChecked( MarbleSettings::lockToSubSolarPoint() );

    // View
    m_lastFileOpenPath = MarbleSettings::lastFileOpenDir();

    // Tracking settings
    readTrackingSettings();

    // Load previous route settings
    m_controlView->marbleModel()->routingManager()->readSettings();
    bool const startupWarning = MarbleSettings::showGuidanceModeStartupWarning();
    m_controlView->marbleModel()->routingManager()->setShowGuidanceModeStartupWarning( startupWarning );

    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig();
    if ( sharedConfig->hasGroup( "Routing Profiles" ) ) {
        QList<RoutingProfile> profiles;
        KConfigGroup profilesGroup = sharedConfig->group( "Routing Profiles" );
        int numProfiles = profilesGroup.readEntry( "Num", 0 );
        for ( int i = 0; i < numProfiles; ++i ) {
            KConfigGroup profileGroup = profilesGroup.group( QString( "Profile %0" ).arg(i) );
            QString name = profileGroup.readEntry( "Name", i18n( "Unnamed" ) );
            RoutingProfile profile( name );
            foreach ( const QString& pluginName, profileGroup.groupList() ) {
                KConfigGroup pluginGroup = profileGroup.group( pluginName );
                profile.pluginSettings().insert( pluginName, QHash<QString, QVariant>() );
                foreach ( const QString& key, pluginGroup.keyList() ) {
                    if (key != QLatin1String("Enabled")) {
                        profile.pluginSettings()[ pluginName ].insert( key, pluginGroup.readEntry( key ) );
                    }
                }
            }
            profiles << profile;
        }
        m_controlView->marbleModel()->routingManager()->profilesModel()->setProfiles( profiles );
    } else {
        m_controlView->marbleModel()->routingManager()->profilesModel()->loadDefaultProfiles();
    }
    int const profileIndex = MarbleSettings::currentRoutingProfile();
    if ( profileIndex >= 0 && profileIndex < m_controlView->marbleModel()->routingManager()->profilesModel()->rowCount() ) {
        RoutingProfile profile = m_controlView->marbleModel()->routingManager()->profilesModel()->profiles().at( profileIndex );
        m_controlView->marbleModel()->routingManager()->routeRequest()->setRoutingProfile( profile );
    }

    PositionTracking *const tracking = m_controlView->marbleModel()->positionTracking();
    tracking->readSettings();
    QString positionProvider = MarbleSettings::activePositionTrackingPlugin();
    if ( !positionProvider.isEmpty() ) {
        const PluginManager* pluginManager = m_controlView->marbleModel()->pluginManager();
        foreach( const PositionProviderPlugin* plugin, pluginManager->positionProviderPlugins() ) {
            if ( plugin->nameId() == positionProvider ) {
                PositionProviderPlugin* instance = plugin->newInstance();
                tracking->setPositionProviderPlugin( instance );
                break;
            }
        }
    }

    readStatusBarSettings();

    updateSettings();

    // Time
    if( MarbleSettings::systemTime() == true  )
    {
        /* nothing to do */
    }
    else if( MarbleSettings::lastSessionTime() == true )
    {
        m_controlView->marbleModel()->setClockDateTime( MarbleSettings::dateTime() );
        m_controlView->marbleModel()->setClockSpeed( MarbleSettings::speedSlider() );
    }

    readPluginSettings();

    m_controlView->setExternalMapEditor( m_externalEditorMapping[MarbleSettings::externalMapEditor()] );

    CloudSyncManager* cloudSyncManager = m_controlView->cloudSyncManager();
    cloudSyncManager->setOwncloudCredentials( MarbleSettings::owncloudServer(),
                                              MarbleSettings::owncloudUsername(),
                                              MarbleSettings::owncloudPassword());
    cloudSyncManager->setSyncEnabled( MarbleSettings::enableSync() );
    cloudSyncManager->routeSyncManager()->setRouteSyncEnabled( MarbleSettings::syncRoutes() );
    cloudSyncManager->bookmarkSyncManager()->setBookmarkSyncEnabled( MarbleSettings::syncBookmarks() );
}

void MarblePart::readTrackingSettings()
{
    if( MarbleSettings::autoCenter() || MarbleSettings::recenterMode() ) {
        CurrentLocationWidget *trackingWidget = m_controlView->currentLocationWidget();
        if( trackingWidget ) {
            trackingWidget->setRecenterMode( MarbleSettings::recenterMode() );
            trackingWidget->setAutoZoom( MarbleSettings::autoZoom() );
            trackingWidget->setTrackVisible( MarbleSettings::trackVisible() );
            trackingWidget->setLastOpenPath( MarbleSettings::lastTrackOpenPath() );
            trackingWidget->setLastSavePath( MarbleSettings::lastTrackSavePath() );
        }
    }
}

void MarblePart::readStatusBarSettings()
{
    const bool showPos = MarbleSettings::showPositionLabel();
    m_showPositionAction->setChecked( showPos );
    showPositionLabel( showPos );

    const bool showAlt = MarbleSettings::showAltitudeLabel();
    m_showAltitudeAction->setChecked( showAlt );
    showAltitudeLabel( showAlt );

    const bool showTileZoom = MarbleSettings::showTileZoomLevelLabel();
    m_showTileZoomLevelAction->setChecked( showTileZoom );
    showTileZoomLevelLabel( showTileZoom );

    const bool showDateTime = MarbleSettings::showDateTimeLabel();
    m_showDateTimeAction->setChecked( showDateTime );
    showDateTimeLabel( showDateTime );

    const bool showProgress = MarbleSettings::showDownloadProgressBar();
    m_showDownloadProgressAction->setChecked( showProgress );
    showDownloadProgressBar( showProgress );
}

void MarblePart::writeSettings()
{
    // Get the 'quit' values from the widget and store them in the settings.
    qreal  quitLon = m_controlView->marbleWidget()->lookAt().longitude();
    qreal  quitLat = m_controlView->marbleWidget()->lookAt().latitude();
    qreal  quitRange = m_controlView->marbleWidget()->lookAt().range();

    MarbleSettings::setQuitLongitude( quitLon );
    MarbleSettings::setQuitLatitude( quitLat );
    MarbleSettings::setQuitRange( quitRange );

    // Get the 'home' values from the widget and store them in the settings.
    qreal  homeLon = 0;
    qreal  homeLat = 0;
    int     homeZoom = 0;

    m_controlView->marbleModel()->home( homeLon, homeLat, homeZoom );
    MarbleSettings::setHomeLongitude( homeLon );
    MarbleSettings::setHomeLatitude( homeLat );
    MarbleSettings::setHomeZoom( homeZoom );

    // Set default font
    MarbleSettings::setMapFont( m_controlView->marbleWidget()->defaultFont() );

    // Get whether animations to the target are enabled
    MarbleSettings::setAnimateTargetVoyage( m_controlView->marbleWidget()->animationsEnabled() );

    // Map theme and projection
    MarbleSettings::setMapTheme( m_controlView->marbleWidget()->mapThemeId() );
    MarbleSettings::setProjection( m_controlView->marbleWidget()->projection() );

    MarbleSettings::setShowClouds( m_controlView->marbleWidget()->showClouds() );

    MarbleSettings::setWorkOffline( m_workOfflineAction->isChecked() );

    MarbleSettings::setStillQuality( m_controlView->marbleWidget()->mapQuality( Still ) );
    MarbleSettings::setAnimationQuality( m_controlView->marbleWidget()->
                                         mapQuality( Animation ) );

    MarbleSettings::setShowBookmarks( m_controlView->marbleModel()->bookmarkManager()->showBookmarks() );

    MarbleSettings::setLastFileOpenDir( m_lastFileOpenPath );

    MarbleSettings::setDistanceUnit( MarbleGlobal::getInstance()->locale()->measurementSystem() );
    MarbleSettings::setAngleUnit( m_controlView->marbleWidget()->defaultAngleUnit() );

    // Sun
    MarbleSettings::setShowSun( m_controlView->marbleWidget()->showSunShading() );
    MarbleSettings::setShowCitylights( m_controlView->marbleWidget()->showCityLights() );
    MarbleSettings::setLockToSubSolarPoint( m_controlView->marbleWidget()->isLockedToSubSolarPoint() );
    MarbleSettings::setSubSolarPointIconVisible( m_controlView->marbleWidget()->isSubSolarPointIconVisible() );

    // Tracking
    CurrentLocationWidget *trackingWidget = m_controlView->currentLocationWidget();
    if( trackingWidget )
    {
        MarbleSettings::setRecenterMode( trackingWidget->recenterMode() );
        MarbleSettings::setAutoZoom( trackingWidget->autoZoom() );
        MarbleSettings::setTrackVisible( trackingWidget->trackVisible() );
        MarbleSettings::setLastTrackOpenPath( trackingWidget->lastOpenPath() );
        MarbleSettings::setLastTrackSavePath( trackingWidget->lastSavePath() );
    }

    // Caches
    MarbleSettings::setVolatileTileCacheLimit( m_controlView->marbleWidget()->
                                               volatileTileCacheLimit() / 1024 );
    MarbleSettings::setPersistentTileCacheLimit( m_controlView->marbleModel()->
                                                 persistentTileCacheLimit() / 1024 );

    // Time
    MarbleSettings::setDateTime( m_controlView->marbleModel()->clockDateTime() );
    MarbleSettings::setSpeedSlider( m_controlView->marbleModel()->clockSpeed() );

    // Plugins
    writePluginSettings();

    QString positionProvider;
    PositionTracking* tracking = m_controlView->marbleModel()->positionTracking();
    tracking->writeSettings();
    if ( tracking->positionProviderPlugin() ) {
        positionProvider = tracking->positionProviderPlugin()->nameId();
    }
    MarbleSettings::setActivePositionTrackingPlugin( positionProvider );

    MarbleSettings::setLockFloatItemPositions( m_lockFloatItemsAct->isChecked() );

    writeStatusBarSettings();

    // Store recent files
    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig();
    m_recentFilesAction->saveEntries( sharedConfig->group( "RecentFiles" ) );

    // Store current route settings
    RoutingManager *routingManager = m_controlView->marbleWidget()->model()->routingManager();
    routingManager->writeSettings();
    bool const startupWarning = routingManager->showGuidanceModeStartupWarning();
    MarbleSettings::setShowGuidanceModeStartupWarning( startupWarning );
    QList<RoutingProfile>  profiles = routingManager->profilesModel()->profiles();
    RoutingProfile const profile = routingManager->routeRequest()->routingProfile();
    MarbleSettings::setCurrentRoutingProfile( profiles.indexOf( profile ) );

    QList<QString> const editors = m_externalEditorMapping.values();
    MarbleSettings::setExternalMapEditor( editors.indexOf( m_controlView->externalMapEditor() ) );

    applyPluginState();

    MarbleSettings::self()->save();
}

void MarblePart::writeStatusBarSettings()
{
    MarbleSettings::setShowPositionLabel( m_showPositionAction->isChecked() );
    MarbleSettings::setShowAltitudeLabel( m_showAltitudeAction->isChecked() );
    MarbleSettings::setShowTileZoomLevelLabel( m_showTileZoomLevelAction->isChecked() );
    MarbleSettings::setShowDateTimeLabel( m_showDateTimeAction->isChecked() );
    MarbleSettings::setShowDownloadProgressBar( m_showDownloadProgressAction->isChecked() );
}

void MarblePart::setupActions()
{
    // Action: Recent Files
    m_recentFilesAction = KStandardAction::openRecent( this, SLOT(openUrl(QUrl)),
                                                       actionCollection() );
    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig();
    m_recentFilesAction->loadEntries( sharedConfig->group( "RecentFiles" ) );

    // Action: Download Region
    m_downloadRegionAction = new QAction( this );
    m_downloadRegionAction->setText( i18nc( "Action for downloading an entire region of a map",
                                            "Download Region..." ));
    actionCollection()->addAction( "file_download_region", m_downloadRegionAction );
    connect( m_downloadRegionAction, SIGNAL(triggered()), SLOT(showDownloadRegionDialog()));

    // Action: Print Map
    m_printMapAction = KStandardAction::print( this, SLOT(printMapScreenShot()),
                                               actionCollection() );

    m_printPreviewAction = KStandardAction::printPreview( m_controlView, SLOT(printPreview()),
                                               actionCollection() );

    // Action: Export Map
    m_exportMapAction = new QAction( this );
    actionCollection()->addAction( "exportMap", m_exportMapAction );
    m_exportMapAction->setText( i18nc( "Action for saving the map to a file", "&Export Map..." ) );
    m_exportMapAction->setIcon( QIcon::fromTheme( "document-save-as" ) );
    actionCollection()->setDefaultShortcut( m_exportMapAction, Qt::CTRL + Qt::Key_S );
    connect( m_exportMapAction, SIGNAL(triggered(bool)),
             this,              SLOT(exportMapScreenShot()) );

    // Action: Work Offline
    m_workOfflineAction = new QAction( this );
    actionCollection()->addAction( "workOffline", m_workOfflineAction );
    m_workOfflineAction->setText( i18nc( "Action for toggling offline mode", "&Work Offline" ) );
    m_workOfflineAction->setIcon( QIcon::fromTheme( "user-offline" ) );
    m_workOfflineAction->setCheckable( true );
    m_workOfflineAction->setChecked( false );
    connect( m_workOfflineAction, SIGNAL(triggered(bool)),
             this,                SLOT(workOffline(bool)) );

    // Action: Copy Map to the Clipboard
    m_copyMapAction = KStandardAction::copy( this, SLOT(copyMap()),
                                             actionCollection() );
    m_copyMapAction->setText( i18nc( "Action for copying the map to the clipboard", "&Copy Map" ) );

    // Action: Copy Coordinates string
    m_copyCoordinatesAction = new QAction( this );
    actionCollection()->addAction( "edit_copy_coordinates",
                                   m_copyCoordinatesAction );
    m_copyCoordinatesAction->setText( i18nc( "Action for copying the coordinates to the clipboard",
                                             "C&opy Coordinates" ) );
    m_copyCoordinatesAction->setIcon(QIcon(QStringLiteral(":/icons/copy-coordinates.png")));
    connect( m_copyCoordinatesAction, SIGNAL(triggered(bool)),
             this,                    SLOT(copyCoordinates()) );

    // Action: Open a Gpx or a Kml File
    m_openAct = KStandardAction::open( this, SLOT(openFile()),
                                       actionCollection() );
    m_openAct->setText( i18nc( "Action for opening a file", "&Open..." ) );

    // Standard actions.  So far only Quit.
    KStandardAction::quit( qApp, SLOT(closeAllWindows()),
                           actionCollection() );

    // Action: Get hot new stuff
    m_newStuffAction = KNS3::standardAction( i18nc( "Action for downloading maps (GHNS)",
                                                    "Download Maps..."),
                                             this,
                                             SLOT(showNewStuffDialog()),
                                             actionCollection(), "new_stuff" );
    m_newStuffAction->setStatusTip( i18nc( "Status tip", "Download new maps"));
    actionCollection()->setDefaultShortcut( m_newStuffAction, Qt::CTRL + Qt::Key_N );

    // Action: Create a New Map
    m_mapWizardAct = new QAction( i18nc( "Action for creating new maps",
                                         "&Create a New Map..." ),
                                  this );
    m_mapWizardAct->setIcon(QIcon(QStringLiteral(":/icons/create-new-map.png")));
    actionCollection()->addAction( "createMap", m_mapWizardAct );
    m_mapWizardAct->setStatusTip( i18nc( "Status tip",
                                         "A wizard guides you through the creation of your own map theme." ) );
    connect( m_mapWizardAct, SIGNAL(triggered()), SLOT(showMapWizard()) );

    KStandardAction::showStatusbar( this, SLOT(showStatusBar(bool)),
                                    actionCollection() );

    m_fullScreenAct = KStandardAction::fullScreen( 0, 0, widget(),
                                                   actionCollection() );
    connect( m_fullScreenAct, SIGNAL(triggered(bool)),
             this,            SLOT(showFullScreen(bool)) );

    // Action: Show Crosshairs option
    QList<RenderPlugin *> pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        if ((*i)->nameId() == QLatin1String("crosshairs")) {
            actionCollection()->addAction( "show_crosshairs", (*i)->action() );
        }
    }

    // Action: Show Clouds option
    m_showCloudsAction = new QAction( this );
    actionCollection()->addAction( "show_clouds", m_showCloudsAction );
    m_showCloudsAction->setCheckable( true );
    m_showCloudsAction->setChecked( true );
    m_showCloudsAction->setIcon(QIcon(QStringLiteral(":/icons/clouds.png")));
    m_showCloudsAction->setText( i18nc( "Action for toggling clouds", "&Clouds" ) );
    connect( m_showCloudsAction, SIGNAL(triggered(bool)),
             this,               SLOT(setShowClouds(bool)) );

    // Action: Show Sunshade options
    m_controlSunAction = new QAction( this );
    actionCollection()->addAction( "control_sun", m_controlSunAction );
    m_controlSunAction->setText( i18nc( "Action for sun control dialog", "S&un Control..." ) );
    connect( m_controlSunAction, SIGNAL(triggered(bool)),
             this,               SLOT(controlSun()) );

    KStandardAction::redisplay( m_controlView->marbleWidget(), SLOT(reloadMap()), actionCollection() );

    // Action: Show Time options
    m_controlTimeAction = new QAction( this );
    actionCollection()->addAction( "control_time", m_controlTimeAction );
    m_controlTimeAction->setIcon(QIcon(QStringLiteral(":/icons/clock.png")));
    m_controlTimeAction->setText( i18nc( "Action for time control dialog", "&Time Control..." ) );
    connect( m_controlTimeAction, SIGNAL(triggered(bool)),
         this,               SLOT(controlTime()) );

    // Action: Lock float items
    m_lockFloatItemsAct = new QAction ( this );
    actionCollection()->addAction( "options_lock_floatitems",
                                   m_lockFloatItemsAct );
    m_lockFloatItemsAct->setText( i18nc( "Action for locking float items on the map",
                                         "Lock Position" ) );
    m_lockFloatItemsAct->setIcon(QIcon(QStringLiteral(":/icons/unlock.png")));
    m_lockFloatItemsAct->setCheckable( true );
    m_lockFloatItemsAct->setChecked( false );
    connect( m_lockFloatItemsAct, SIGNAL(triggered(bool)),
             this,                SLOT(lockFloatItemPosition(bool)) );

    KStandardAction::preferences( this, SLOT(editSettings()),
                                  actionCollection() );

    //Toggle Action: Show sun shadow
    m_showShadow = new KToggleAction( i18n( "Show Shadow" ), this );
//     m_showShadow->setIcon(QIcon(QStringLiteral("")));        // Fixme: Add Icon
    actionCollection()->addAction( "sun_shadow", m_showShadow );
    m_showShadow->setCheckedState( KGuiItem( i18n( "Hide Shadow" ) ) );
    m_showShadow->setToolTip(i18n("Shows and hides the shadow of the sun"));
    connect( m_showShadow, SIGNAL(triggered(bool)), this, SLOT(showSun(bool)));

    //Toggle Action: Show Sun icon on the Sub-Solar Point
    m_setSubSolarPointIconVisible = new KToggleAction( i18n( "Show sun icon on the Sub-Solar Point" ), this );
    actionCollection()->addAction( "show_icon_on_subsolarpoint", m_setSubSolarPointIconVisible );
    m_setSubSolarPointIconVisible->setCheckedState( KGuiItem( i18n( "Hide sun icon on the Sub-Solar Point" ) ) );
    m_setSubSolarPointIconVisible->setToolTip( i18n( "Show sun icon on the sub-solar point" ) );
    connect( m_setSubSolarPointIconVisible, SIGNAL(triggered(bool)), this, SLOT(setSubSolarPointIconVisible(bool)));


    //Toggle Action: Lock globe to the Sub-Solar Point
    m_lockToSubSolarPoint = new KToggleAction( i18n( "Lock Globe to the Sub-Solar Point" ), this );
    actionCollection()->addAction( "lock_to_subsolarpoint", m_lockToSubSolarPoint );
    m_lockToSubSolarPoint->setCheckedState( KGuiItem( i18n( "Unlock Globe to the Sub-Solar Point" ) ) );
    m_lockToSubSolarPoint->setToolTip( i18n( "Lock globe to the sub-solar point" ) );
    connect( m_lockToSubSolarPoint, SIGNAL(triggered(bool)), this, SLOT(lockToSubSolarPoint(bool)));

    //    FIXME: Discuss if this is the best place to put this
    QList<RenderPlugin *>::const_iterator it = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const itEnd = pluginList.constEnd();
    for (; it != itEnd; ++it ) {
        if ((*it)->nameId() != QLatin1String("annotation")) {
            connect( (*it), SIGNAL(actionGroupsChanged()),
                     this, SLOT(createPluginMenus()) );
        }
    }

    m_addBookmarkAction = new QAction( this );
    actionCollection()->addAction( "add_bookmark", m_addBookmarkAction );
    m_addBookmarkAction->setText( i18nc( "Add Bookmark", "&Add Bookmark" ) );
    m_addBookmarkAction->setIcon(QIcon(QStringLiteral(":/icons/bookmark-new.png")));
    actionCollection()->setDefaultShortcut( m_addBookmarkAction, Qt::CTRL + Qt::Key_B );
    connect( m_addBookmarkAction, SIGNAL(triggered()),
             this,                SLOT(openEditBookmarkDialog()) );

    m_toggleBookmarkDisplayAction = new QAction( this );
    actionCollection()->addAction( "show_bookmarks", m_toggleBookmarkDisplayAction );
    m_toggleBookmarkDisplayAction->setText( i18nc( "Show Bookmarks", "Show &Bookmarks" ) );
    m_toggleBookmarkDisplayAction->setStatusTip( i18n( "Show or hide bookmarks in the map" ) );
    m_toggleBookmarkDisplayAction->setCheckable( true );
    m_toggleBookmarkDisplayAction->setChecked( m_controlView->marbleModel()->bookmarkManager()->showBookmarks() );
    connect( m_toggleBookmarkDisplayAction, SIGNAL(toggled(bool)),
                  m_controlView->marbleModel()->bookmarkManager(), SLOT(setShowBookmarks(bool)) );

    m_setHomeAction = new QAction( this );
    actionCollection()->addAction( "set_home", m_setHomeAction );
    m_setHomeAction->setText( i18n( "&Set Home Location" ) );
    m_setHomeAction->setIcon( QIcon::fromTheme( "go-home" ) );
    connect( m_setHomeAction, SIGNAL(triggered()),
             this,                SLOT(setHome()) );

    m_manageBookmarksAction = new QAction( this );
    actionCollection()->addAction( "manage_bookmarks", m_manageBookmarksAction );
    m_manageBookmarksAction->setText( i18nc( "Manage Bookmarks", "&Manage Bookmarks" ) );
    m_manageBookmarksAction->setIcon(QIcon(QStringLiteral(":/icons/bookmarks-organize.png")));
    connect( m_manageBookmarksAction, SIGNAL(triggered()),
             this,                SLOT(openManageBookmarksDialog()) );


    createFolderList();
    connect( m_controlView->marbleModel()->bookmarkManager(),
             SIGNAL(bookmarksChanged()), this, SLOT(createFolderList()) );

    m_externalMapEditorAction = new QAction( this );
    actionCollection()->addAction( "external_editor", m_externalMapEditorAction );
    m_externalMapEditorAction->setText( i18nc( "Edit the map in an external application", "&Edit Map" ) );
    m_externalMapEditorAction->setIcon(QIcon(QStringLiteral(":/icons/edit-map.png")));
    actionCollection()->setDefaultShortcut( m_externalMapEditorAction, Qt::CTRL + Qt::Key_E );
    connect( m_externalMapEditorAction, SIGNAL(triggered()),
             m_controlView, SLOT(launchExternalMapEditor()) );
    connect( m_controlView->marbleWidget(), SIGNAL(themeChanged(QString)),
             this, SLOT(updateMapEditButtonVisibility(QString)) );

     m_recordMovieAction = new QAction( i18n( "&Record Movie" ), this );
     actionCollection()->addAction( "record_movie" , m_recordMovieAction );
     m_recordMovieAction->setStatusTip( i18n( "Records a movie of the globe" ) );
     actionCollection()->setDefaultShortcut( m_recordMovieAction, Qt::CTRL + Qt::SHIFT + Qt::Key_R );
     m_recordMovieAction->setIcon(QIcon(QStringLiteral(":/icons/animator.png")));
     connect( m_recordMovieAction, SIGNAL(triggered()),
             this, SLOT(showMovieCaptureDialog()) );

     m_stopRecordingAction = new QAction( i18n( "&Stop recording" ), this );
     actionCollection()->addAction( "stop_recording" , m_stopRecordingAction );
     m_stopRecordingAction->setStatusTip( i18n( "Stop recording a movie of the globe" ) );
     actionCollection()->setDefaultShortcut( m_recordMovieAction, Qt::CTRL + Qt::SHIFT + Qt::Key_S );
     m_stopRecordingAction->setEnabled( false );
     connect( m_stopRecordingAction, SIGNAL(triggered()),
             this, SLOT(stopRecording()) );
}

void MarblePart::createFolderList()
{

    QList<QAction*> actionList;

   QVector<GeoDataFolder*> folders = m_controlView->marbleModel()->bookmarkManager()->folders();
   QVector<GeoDataFolder*>::const_iterator i = folders.constBegin();
   QVector<GeoDataFolder*>::const_iterator end = folders.constEnd();

   for (; i != end; ++i ) {
       QMenu *m_bookmarksListMenu = new QMenu( (*i)->name() );

       createBookmarksListMenu( m_bookmarksListMenu, *(*i) );
       connect( m_bookmarksListMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(lookAtBookmark(QAction*)) );

       actionList.append( m_bookmarksListMenu->menuAction() );
   }
   unplugActionList("folders");
   plugActionList( "folders", actionList );
}

void MarblePart::createBookmarksListMenu( QMenu *m_bookmarksListMenu, const GeoDataFolder &folder )
{
    m_bookmarksListMenu->clear();
    QVector<GeoDataPlacemark*> bookmarks = folder.placemarkList();

    QVector<GeoDataPlacemark*>::const_iterator i = bookmarks.constBegin();
    QVector<GeoDataPlacemark*>::const_iterator end = bookmarks.constEnd();

    for (; i != end; ++i ) {
        QAction *bookmarkAct = new QAction( (*i)->name(), this );
        QVariant var;
        GeoDataLookAt* lookAt = (*i)->lookAt();
        if ( !lookAt ) {
            GeoDataLookAt coordinateToLookAt;
            coordinateToLookAt.setCoordinates( (*i)->coordinate() );
            coordinateToLookAt.setRange( (*i)->coordinate().altitude() );
        } else {
            var.setValue( *lookAt );
        }
        bookmarkAct->setData( var );
        m_bookmarksListMenu->addAction( bookmarkAct );
    }

}

void MarblePart::createInfoBoxesMenu()
{
    QList<AbstractFloatItem *> floatItemList = m_controlView->marbleWidget()->floatItems();

    QList<QAction*> actionList;

    QList<AbstractFloatItem *>::const_iterator i = floatItemList.constBegin();
    QList<AbstractFloatItem *>::const_iterator const end = floatItemList.constEnd();
    for (; i != end; ++i ) {
        actionList.append( (*i)->action() );
    }

    unplugActionList( "infobox_actionlist" );
    plugActionList( "infobox_actionlist", actionList );
}

void MarblePart::createOnlineServicesMenu()
{
    QList<RenderPlugin *> renderPluginList = m_controlView->marbleWidget()->renderPlugins();

    QList<QAction*> actionList;

    QList<RenderPlugin *>::const_iterator i = renderPluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = renderPluginList.constEnd();
    for (; i != end; ++i ) {
        // FIXME: This will go into the layer manager when AbstractDataPlugin is an interface

        if( (*i)->renderType() == RenderPlugin::OnlineRenderType ) {
            actionList.append( (*i)->action() );
        }
    }

    unplugActionList( "onlineservices_actionlist" );
    plugActionList( "onlineservices_actionlist", actionList );
}

void MarblePart::createRenderPluginActions()
{
    QList<RenderPlugin *> renderPluginList = m_controlView->marbleWidget()->renderPlugins();

    QList<QAction*> actionList;

    QList<RenderPlugin *>::const_iterator i = renderPluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = renderPluginList.constEnd();
    for (; i != end; ++i ) {
        if( (*i)->renderType() == RenderPlugin::ThemeRenderType ) {
            actionList.append( (*i)->action() );
        }
    }

    unplugActionList( "themerender_actionlist" );
    plugActionList( "themerender_actionlist", actionList );
}

void MarblePart::showDateTime()
{
    m_clock = QLocale().toString( m_controlView->marbleModel()->clockDateTime().addSecs( m_controlView->marbleModel()->clockTimezone() ), QLocale::ShortFormat );
    updateStatusBar();
}

void MarblePart::showPosition( const QString& position )
{
    m_position = position;
    updateStatusBar();
}

void MarblePart::showZoomLevel( const int tileLevel )
{
    if ( tileLevel == -1 )
        m_tileZoomLevel = QCoreApplication::translate( "Marble", NOT_AVAILABLE );
    else {
        m_tileZoomLevel.setNum( tileLevel );
    }
    updateStatusBar();
}

void MarblePart::mapThemeChanged( const QString& newMapTheme )
{
    Q_UNUSED( newMapTheme );
    updateTileZoomLevel();
    updateStatusBar();
}

void MarblePart::createPluginMenus()
{
    unplugActionList("plugins_actionlist");
    unplugActionList("plugins_menuactionlist");

    QList<RenderPlugin *> renderPluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = renderPluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = renderPluginList.constEnd();

    for (; i != end; ++i ) {
        // menus
        const QList<QActionGroup*> *tmp_actionGroups = (*i)->actionGroups();
        if( (*i)->enabled() && tmp_actionGroups ) {
            foreach( QActionGroup *ag, *tmp_actionGroups ) {
                plugActionList( "plugins_menuactionlist", ag->actions() );
            }
        }

        // toolbars
        const QList<QActionGroup*> *tmp_toolbarActionGroups = (*i)->toolbarActionGroups();
        if ( (*i)->enabled() && tmp_toolbarActionGroups ) {

            foreach( QActionGroup* ag, *tmp_toolbarActionGroups ) {
                plugActionList( "plugins_actionlist", ag->actions() );
            }
        }
    }

}

void MarblePart::updateTileZoomLevel()
{
    const int tileZoomLevel =
        m_controlView->marbleWidget()->tileZoomLevel();
    if ( tileZoomLevel == -1 )
        m_tileZoomLevel = QCoreApplication::translate( "Marble", NOT_AVAILABLE );
    else {
        m_tileZoomLevel.setNum( tileZoomLevel );
    }
}

void MarblePart::migrateNewstuffConfigFiles() const
{
    // Newstuff config files used to be in the KDE data directory of the user, but are now
    // shared between Marble KDE and Marble Qt in Marble's data path of the user.
    // This method moves an old KDE newstuff config file to the new location if the former
    // exists and the latter not.
    QFileInfo const target(MarbleDirs::localPath() + QLatin1String("/newstuff/marble-map-themes.knsregistry"));
    if ( !target.exists() ) {
        QString const source = QStandardPaths::locate( QStandardPaths::GenericDataLocation, "knewstuff3/marble.knsregistry" );
        if ( !source.isEmpty() ) {
            if ( !target.absoluteDir().exists() ) {
                if ( !QDir::root().mkpath( target.absolutePath() ) ) {
                    mDebug() << "Failed to create target directory " << target.absolutePath() << " needed for newstuff migration";
                    return;
                }
            }

            QFile registryFile( source );
            if ( !registryFile.open( QFile::ReadOnly ) ) {
                mDebug() << "Cannot parse newstuff xml file";
                return;
            }
            QDomDocument xml;
            if ( !xml.setContent( registryFile.readAll() ) ) {
                mDebug() << "Cannot parse newstuff xml data";
                return;
            }

            QDomNodeList items = xml.elementsByTagName( "stuff" );
            for ( int i = 0; i < items.length(); ++i ) {
                repairNode( items.item(i), QString("summary") );
                repairNode( items.item(i), QString("author") );
            }

            QFile output( target.absoluteFilePath() );
            if ( !output.open( QFile::WriteOnly ) ) {
                mDebug() << "Cannot open " << target.absoluteFilePath() << " for writing";
            } else {
                QTextStream outStream( &output );
                outStream << xml.toString( 2 );
                outStream.flush();
                output.close();
            }
        }
    }
}

void MarblePart::repairNode( QDomNode node, const QString &child )
{
    int const size = node.namedItem( child ).toElement().text().size();
    if ( size > 1024 ) {
        QString const theme = node.namedItem(QStringLiteral("name")).toElement().text();
        mDebug() << "Removing GHNS field " << child << " of map theme " << theme << ": Size " << size << " exceeds maximum size (see bug 319542).";
        node.removeChild( node.namedItem( child ) );
    }
}

void MarblePart::updateCloudSyncStatus(const QString& status )
{
    m_ui_cloudSyncSettings.cloudSyncStatus->setText(status);
    switch (m_controlView->cloudSyncManager()->status()){
        case CloudSyncManager::Success:
            m_ui_cloudSyncSettings.cloudSyncStatus->setStyleSheet("QLabel { color : green; }");
            break;
        case CloudSyncManager::Error:
            m_ui_cloudSyncSettings.cloudSyncStatus->setStyleSheet("QLabel { color : red; }");
            break;
        case CloudSyncManager::Unknown:
            m_ui_cloudSyncSettings.cloudSyncStatus->setStyleSheet("QLabel { color : grey; }");
            break;
    }
}

void MarblePart::updateCloudSyncCredentials()
{
    m_controlView->cloudSyncManager()->setOwncloudCredentials( m_ui_cloudSyncSettings.kcfg_owncloudServer->text(),
                                                               m_ui_cloudSyncSettings.kcfg_owncloudUsername->text(),
                                                               m_ui_cloudSyncSettings.kcfg_owncloudPassword->text() );
}

void MarblePart::updateStatusBar()
{
    if ( m_positionLabel )
        m_positionLabel->setText( i18n( POSITION_STRING, m_position ) );

    if ( m_distanceLabel )
        m_distanceLabel->setText( i18n( DISTANCE_STRING, m_controlView->marbleWidget()->distanceString() ) );

    if ( m_tileZoomLevelLabel )
        m_tileZoomLevelLabel->setText( i18n( TILEZOOMLEVEL_STRING,
                                             m_tileZoomLevel ) );

    if ( m_clockLabel )
        m_clockLabel->setText( i18n( DATETIME_STRING, m_clock ) );
}

void MarblePart::setupStatusBar()
{
    QFontMetrics statusBarFontMetrics( m_statusBarExtension->statusBar()->fontMetrics() );

    // UTM syntax is used in the template string, as it is longer than the lon/lat one
    QString templatePositionString =
        QString( "%1 00Z 000000.00 m E, 00000000.00 m N_" ).arg(POSITION_STRING);
    m_positionLabel = setupStatusBarLabel( templatePositionString );

    QString templateDistanceString =
        QString( "%1 00.000,0 mu" ).arg(DISTANCE_STRING);
    m_distanceLabel = setupStatusBarLabel( templateDistanceString );

    QString templateDateTimeString = QString( "%1 %2" ).arg( DATETIME_STRING , QLocale().toString( QDateTime::fromString ( "01:01:1000", "dd:mm:yyyy"), QLocale::ShortFormat ) );

    m_clockLabel = setupStatusBarLabel( templateDateTimeString );

    const QString templateTileZoomLevelString = i18n( TILEZOOMLEVEL_STRING, m_tileZoomLevel );
    m_tileZoomLevelLabel = setupStatusBarLabel( templateTileZoomLevelString );

    connect( m_controlView->marbleWidget(), SIGNAL(mouseMoveGeoPosition(QString)),
             this,                          SLOT(showPosition(QString)) );
    connect( m_controlView->marbleWidget(), SIGNAL(distanceChanged(QString)),
             this,                          SLOT(updateStatusBar()) );
    connect( m_controlView->marbleWidget(), SIGNAL(tileLevelChanged(int)),
             SLOT(showZoomLevel(int)));
    connect( m_controlView->marbleWidget(), SIGNAL(themeChanged(QString)),
             this, SLOT(mapThemeChanged(QString)), Qt::QueuedConnection );
    connect( m_controlView->marbleModel()->clock(), SIGNAL(timeChanged()),
             this,                          SLOT(showDateTime()) );


    setupDownloadProgressBar();

    setupStatusBarActions();
    updateStatusBar();
}

QLabel * MarblePart::setupStatusBarLabel( const QString& templateString )
{
    QFontMetrics statusBarFontMetrics( m_statusBarExtension->statusBar()->fontMetrics() );

    QLabel * const label = new QLabel( m_statusBarExtension->statusBar() );
    label->setIndent( 5 );
    int maxWidth = statusBarFontMetrics.boundingRect( templateString ).width()
        + 2 * label->margin() + 2 * label->indent();
    label->setFixedWidth( maxWidth );
    m_statusBarExtension->addStatusBarItem( label, -1, false );
    return label;
}

void MarblePart::setupDownloadProgressBar()
{
    // get status bar and add progress widget
    QStatusBar * const statusBar = m_statusBarExtension->statusBar();
    Q_ASSERT( statusBar );

    m_downloadProgressBar = new QProgressBar;
    m_downloadProgressBar->setVisible( MarbleSettings::showDownloadProgressBar() );
    statusBar->addPermanentWidget( m_downloadProgressBar );

    HttpDownloadManager * const downloadManager =
        m_controlView->marbleModel()->downloadManager();
    Q_ASSERT( downloadManager );
    connect( downloadManager, SIGNAL(progressChanged(int,int)), SLOT(handleProgress(int,int)) );
    connect( downloadManager, SIGNAL(jobRemoved()), SLOT(removeProgressItem()) );
}

void MarblePart::setupStatusBarActions()
{
    QStatusBar * const statusBar = m_statusBarExtension->statusBar();
    Q_ASSERT( statusBar );

    statusBar->setContextMenuPolicy( Qt::CustomContextMenu );

    connect( statusBar, SIGNAL(customContextMenuRequested(QPoint)),
             this, SLOT(showStatusBarContextMenu(QPoint)));

    m_showPositionAction = new KToggleAction( i18nc( "Action for toggling", "Show Position" ),
                                              this );
    m_showDateTimeAction = new KToggleAction( i18nc( "Action for toggling", "Show Date and Time" ),
                                              this );
    m_showAltitudeAction = new KToggleAction( i18nc( "Action for toggling", "Show Altitude" ),
                                              this );
    m_showTileZoomLevelAction = new KToggleAction( i18nc( "Action for toggling",
                                                          "Show Tile Zoom Level" ), this );
    m_showDownloadProgressAction = new KToggleAction( i18nc( "Action for toggling",
                                                             "Show Download Progress Bar" ), this );

    connect( m_showPositionAction, SIGNAL(triggered(bool)),
             this, SLOT(showPositionLabel(bool)) );
    connect( m_showAltitudeAction, SIGNAL(triggered(bool)),
             this, SLOT(showAltitudeLabel(bool)) );
    connect( m_showTileZoomLevelAction, SIGNAL(triggered(bool)),
             this, SLOT(showTileZoomLevelLabel(bool)) );
    connect( m_showDateTimeAction, SIGNAL(triggered(bool)),
             this, SLOT(showDateTimeLabel(bool)) );
    connect( m_showDownloadProgressAction, SIGNAL(triggered(bool)),
             this, SLOT(showDownloadProgressBar(bool)) );
}

void MarblePart::showNewStuffDialog()
{
    QPointer<MapThemeDownloadDialog> dialog( new MapThemeDownloadDialog( m_controlView->marbleWidget() ) );
    dialog->exec();
    delete dialog;
}

void MarblePart::showUploadNewStuffDialog()
{
    QString  newStuffConfig = QStandardPaths::locate( QStandardPaths::GenericDataLocation, "marble/marble.knsrc" );
    qDebug() << "KNS config file:" << newStuffConfig;

    QPointer<KNS3::UploadDialog> dialog( new KNS3::UploadDialog( newStuffConfig, m_controlView ) );
    qDebug() << "Creating the archive";
    dialog->setUploadFile( QUrl( MapWizard::createArchive( m_controlView, m_controlView->marbleWidget()->mapThemeId() ) ) );
    dialog->exec();
    MapWizard::deleteArchive( m_controlView->marbleWidget()->mapThemeId() );
    delete dialog;
}

void MarblePart::showDownloadRegionDialog()
{
    MarbleWidget * const marbleWidget = m_controlView->marbleWidget();
    if ( !m_downloadRegionDialog ) {
        m_downloadRegionDialog = new DownloadRegionDialog( marbleWidget, widget() );
        // it might be tempting to move the connects to DownloadRegionDialog's "accepted" and
        // "applied" signals, be aware that the "hidden" signal might be come before the "accepted"
        // signal, leading to a too early disconnect.
        connect( m_downloadRegionDialog, SIGNAL(accepted()), SLOT(downloadRegion()));
        connect( m_downloadRegionDialog, SIGNAL(applied()), SLOT(downloadRegion()));
    }
    // FIXME: get allowed range from current map theme
    m_downloadRegionDialog->setAllowedTileLevelRange( 0, 16 );
    m_downloadRegionDialog->setSelectionMethod( DownloadRegionDialog::VisibleRegionMethod );
    ViewportParams const * const viewport = marbleWidget->viewport();
    m_downloadRegionDialog->setSpecifiedLatLonAltBox( viewport->viewLatLonAltBox() );
    m_downloadRegionDialog->setVisibleLatLonAltBox( viewport->viewLatLonAltBox() );
    m_downloadRegionDialog->setVisibleTileLevel( marbleWidget->tileZoomLevel() );

    m_downloadRegionDialog->show();
    m_downloadRegionDialog->raise();
    m_downloadRegionDialog->activateWindow();
}

void MarblePart::downloadRegion()
{
    Q_ASSERT( m_downloadRegionDialog );
    QVector<TileCoordsPyramid> const pyramid = m_downloadRegionDialog->region();
    if ( !pyramid.isEmpty() ) {
        m_controlView->marbleWidget()->downloadRegion( pyramid );
    }
}

void MarblePart::showStatusBarContextMenu( const QPoint& pos )
{
    QStatusBar * const statusBar = m_statusBarExtension->statusBar();
    Q_ASSERT( statusBar );

    QMenu statusBarContextMenu( m_controlView->marbleWidget() );
    statusBarContextMenu.addAction( m_showPositionAction );
    statusBarContextMenu.addAction( m_showDateTimeAction );
    statusBarContextMenu.addAction( m_showAltitudeAction );
    statusBarContextMenu.addAction( m_showTileZoomLevelAction );
    statusBarContextMenu.addAction( m_showDownloadProgressAction );

    statusBarContextMenu.exec( statusBar->mapToGlobal( pos ));
}

void MarblePart::showMapWizard()
{
    // Map Wizard
    QPointer<MapWizard> mapWizard = new MapWizard( m_controlView );
    mapWizard->setWmsServers( MarbleSettings::wmsServers() );
    mapWizard->setStaticUrlServers( MarbleSettings::staticUrlServers() );
    mapWizard->exec();
    MarbleSettings::setWmsServers( mapWizard->wmsServers() );
    MarbleSettings::setStaticUrlServers( mapWizard->staticUrlServers() );
    mapWizard->deleteLater();
}

void MarblePart::editSettings()
{
    if ( KConfigDialog::showDialog( "settings" ) )
        return;

    m_configDialog = new KConfigDialog( m_controlView, "settings",
                                        MarbleSettings::self() );

    // view page
    Ui_MarbleViewSettingsWidget  ui_viewSettings;
    QWidget                     *w_viewSettings = new QWidget( 0 );

    w_viewSettings->setObjectName( "view_page" );
    ui_viewSettings.setupUi( w_viewSettings );
    m_configDialog->addPage( w_viewSettings, i18n( "View" ), "configure" );

    ui_viewSettings.label_labelLocalization->hide();
    ui_viewSettings.kcfg_labelLocalization->hide();

    // navigation page
    Ui_MarbleNavigationSettingsWidget  ui_navigationSettings;
    QWidget                           *w_navigationSettings = new QWidget( 0 );

    w_navigationSettings->setObjectName( "navigation_page" );
    ui_navigationSettings.setupUi( w_navigationSettings );
    m_configDialog->addPage( w_navigationSettings, i18n( "Navigation" ),
                             "transform-move" );
    ui_navigationSettings.kcfg_dragLocation->hide();
    ui_navigationSettings.label_dragLocation->hide();

    // cache page
    Ui_MarbleCacheSettingsWidget  ui_cacheSettings;
    QWidget                      *w_cacheSettings = new QWidget( 0 );

    w_cacheSettings->setObjectName( "cache_page" );
    ui_cacheSettings.setupUi( w_cacheSettings );
    m_configDialog->addPage( w_cacheSettings, i18n( "Cache & Proxy" ),
                             "preferences-web-browser-cache" );
    connect( ui_cacheSettings.button_clearVolatileCache, SIGNAL(clicked()),
             m_controlView->marbleWidget(), SLOT(clearVolatileTileCache()) );
    connect( ui_cacheSettings.button_clearPersistentCache, SIGNAL(clicked()),
             m_controlView->marbleModel(), SLOT(clearPersistentTileCache()) );

    // time page
    Ui_MarbleTimeSettingsWidget ui_timeSettings;
    QWidget *w_timeSettings = new QWidget( 0 );

    w_timeSettings->setObjectName( "time_page" );
    ui_timeSettings.setupUi( w_timeSettings );
    m_configDialog->addPage( w_timeSettings, i18n( "Date & Time" ), "clock" );
    
    // Sync page
    QWidget *w_cloudSyncSettings = new QWidget( 0 );

    w_cloudSyncSettings->setObjectName( "sync_page" );
    m_ui_cloudSyncSettings.setupUi( w_cloudSyncSettings );
    m_ui_cloudSyncSettings.button_syncNow->setEnabled( MarbleSettings::syncBookmarks() );
    m_configDialog->addPage( w_cloudSyncSettings, i18n( "Synchronization" ), "folder-sync" );

    connect( m_ui_cloudSyncSettings.button_syncNow, SIGNAL(clicked()),
             m_controlView->cloudSyncManager()->bookmarkSyncManager(), SLOT(startBookmarkSync()) );
    connect( m_ui_cloudSyncSettings.testLoginButton, SIGNAL(clicked()),
             this, SLOT(updateCloudSyncCredentials()) );

    connect( m_controlView->cloudSyncManager(), SIGNAL(statusChanged(QString)),
             this, SLOT(updateCloudSyncStatus(QString)));

    // routing page
    RoutingProfilesWidget *w_routingSettings = new RoutingProfilesWidget( m_controlView->marbleModel() );
    w_routingSettings->setObjectName( "routing_page" );
    m_configDialog->addPage( w_routingSettings, i18n( "Routing" ), "flag");

    // plugin page
    MarblePluginSettingsWidget *w_pluginSettings = new MarblePluginSettingsWidget();
    RenderPluginModel *const pluginModel = new RenderPluginModel( w_pluginSettings );
    pluginModel->setRenderPlugins( m_controlView->marbleWidget()->renderPlugins() );
    w_pluginSettings->setModel( pluginModel );
    w_pluginSettings->setObjectName( "plugin_page" );
    m_configDialog->addPage( w_pluginSettings, i18n( "Plugins" ),
                             "preferences-plugin" );
    // Setting the icons of the pluginSettings page.
    w_pluginSettings->setConfigIcon( QIcon::fromTheme( "configure" ) );
    w_pluginSettings->setAboutIcon( QIcon::fromTheme( "help-about" ) );

    connect( w_pluginSettings, SIGNAL(pluginListViewClicked()),
                               SLOT(enableApplyButton()) );
    connect( m_configDialog,   SIGNAL(settingsChanged(QString)),
                               SLOT(updateSettings()) );
    connect( m_configDialog,   SIGNAL(accepted()),
                               SLOT(applyPluginState()) );
    connect( m_configDialog,   SIGNAL(accepted()),
             pluginModel,      SLOT(applyPluginState()) );
    connect( m_configDialog,   SIGNAL(rejected()),
             pluginModel,      SLOT(retrievePluginState()) );

    m_configDialog->show();
}

void MarblePart::enableApplyButton()
{
    //m_configDialog->enableButtonApply( true );
}

void MarblePart::applyPluginState()
{
    QList<RoutingProfile>  profiles = m_controlView->marbleWidget()
                        ->model()->routingManager()->profilesModel()->profiles();
    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig();
    KConfigGroup profilesGroup = sharedConfig->group( "Routing Profiles" );
    profilesGroup.writeEntry( "Num", profiles.count() );
    for ( int i = 0; i < profiles.count(); ++i ) {
        KConfigGroup profileGroup = profilesGroup.group( QString( "Profile %0" ).arg(i) );
        RoutingProfile profile = profiles.at( i );
        profileGroup.writeEntry( "Name", profile.name() );
        foreach ( const QString &pluginName, profileGroup.groupList() ) {
            profileGroup.group( pluginName ).deleteGroup();
        }
        foreach ( const QString &key, profile.pluginSettings().keys() ) {
            KConfigGroup pluginGroup = profileGroup.group( key );
            pluginGroup.writeEntry( "Enabled", true );
            foreach ( const QString& settingKey, profile.pluginSettings()[ key ].keys() ) {
                Q_ASSERT(settingKey != QLatin1String("Enabled"));
                pluginGroup.writeEntry( settingKey, profile.pluginSettings()[ key ][ settingKey ] );
            }
        }
    }
}

void MarblePart::updateSettings()
{
    qDebug() << "Updating Settings ...";

    // FIXME: Font doesn't get updated instantly.
    m_controlView->marbleWidget()->setDefaultFont( MarbleSettings::mapFont() );

    m_controlView->marbleWidget()->
        setMapQualityForViewContext( (MapQuality) MarbleSettings::stillQuality(),
                                     Still );
    m_controlView->marbleWidget()->
        setMapQualityForViewContext( (MapQuality) MarbleSettings::animationQuality(),
                                     Animation );

    m_controlView->marbleWidget()->
        setDefaultAngleUnit( (AngleUnit) MarbleSettings::angleUnit() );
    MarbleGlobal::getInstance()->locale()->
        setMeasurementSystem( (MarbleLocale::MeasurementSystem) MarbleSettings::distanceUnit() );

    updateStatusBar();

    m_controlView->marbleWidget()->setAnimationsEnabled( MarbleSettings::animateTargetVoyage() );

    // Cache
    m_controlView->marbleModel()->
        setPersistentTileCacheLimit( MarbleSettings::persistentTileCacheLimit() * 1024 );
    m_controlView->marbleWidget()->
        setVolatileTileCacheLimit( MarbleSettings::volatileTileCacheLimit() * 1024 );

    //Create and export the proxy
    QNetworkProxy proxy;

    // Make sure that no proxy is used for an empty string or the default value:
    if (MarbleSettings::proxyUrl().isEmpty() || MarbleSettings::proxyUrl() == QLatin1String("http://")) {
        proxy.setType( QNetworkProxy::NoProxy );
    } else {
        if ( MarbleSettings::proxyType() == Marble::Socks5Proxy ) {
            proxy.setType( QNetworkProxy::Socks5Proxy );
        }
        else if ( MarbleSettings::proxyType() == Marble::HttpProxy ) {
            proxy.setType( QNetworkProxy::HttpProxy );
        }
        else {
            qDebug() << "Unknown proxy type! Using Http Proxy instead.";
            proxy.setType( QNetworkProxy::HttpProxy );
        }
    }

    proxy.setHostName( MarbleSettings::proxyUrl() );
    proxy.setPort( MarbleSettings::proxyPort() );

    if ( MarbleSettings::proxyAuth() ) {
        proxy.setUser( MarbleSettings::proxyUser() );
        proxy.setPassword( MarbleSettings::proxyPass() );
    }

    QNetworkProxy::setApplicationProxy(proxy);

    m_controlView->marbleWidget()->update();

    // Time
    if( MarbleSettings::systemTimezone() == true  )
    {
        QDateTime localTime = QDateTime::currentDateTime().toLocalTime();
        localTime.setTimeSpec( Qt::UTC );
        m_controlView->marbleModel()->setClockTimezone( QDateTime::currentDateTime().toUTC().secsTo( localTime ) );
    }
    else if( MarbleSettings::utc() == true )
    {
        m_controlView->marbleModel()->setClockTimezone( 0 );
    }
    else if( MarbleSettings::customTimezone() == true )
    {
        m_controlView->marbleModel()->setClockTimezone( m_timezone.value( MarbleSettings::chosenTimezone() ) );
    }

    // Route rendering colors and alpha values
    QColor tempColor;
    tempColor = MarbleSettings::routeColorStandard();
    tempColor.setAlpha( MarbleSettings::routeAlphaStandard() );
    m_controlView->marbleModel()->routingManager()->setRouteColorStandard( tempColor );

    tempColor = MarbleSettings::routeColorHighlighted();
    tempColor.setAlpha( MarbleSettings::routeAlphaHighlighted() );
    m_controlView->marbleModel()->routingManager()->setRouteColorHighlighted( tempColor );

    tempColor = MarbleSettings::routeColorAlternative();
    tempColor.setAlpha( MarbleSettings::routeAlphaAlternative() );
    m_controlView->marbleModel()->routingManager()->setRouteColorAlternative( tempColor );

    // External map editor
    m_controlView->setExternalMapEditor( m_externalEditorMapping[MarbleSettings::externalMapEditor()] );
    m_controlView->marbleWidget()->inputHandler()->setInertialEarthRotationEnabled( MarbleSettings::inertialEarthRotation() );

    CloudSyncManager* cloudSyncManager = m_controlView->cloudSyncManager();
    cloudSyncManager->setOwncloudCredentials( MarbleSettings::owncloudServer(),
                                              MarbleSettings::owncloudUsername(),
                                              MarbleSettings::owncloudPassword());
    cloudSyncManager->setSyncEnabled( MarbleSettings::enableSync() );
    cloudSyncManager->routeSyncManager()->setRouteSyncEnabled( MarbleSettings::syncRoutes() );
    cloudSyncManager->bookmarkSyncManager()->setBookmarkSyncEnabled( MarbleSettings::syncBookmarks() );
}

void MarblePart::writePluginSettings()
{
    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig();

    foreach( RenderPlugin *plugin, m_controlView->marbleWidget()->renderPlugins() ) {
        KConfigGroup group = sharedConfig->group( QString( "plugin_" ) + plugin->nameId() );

        const QHash<QString,QVariant> hash = plugin->settings();

        QHash<QString,QVariant>::const_iterator it = hash.begin();
        while( it != hash.end() ) {
            group.writeEntry( it.key(), it.value() );
            ++it;
        }
        group.sync();
    }
}

void MarblePart::readPluginSettings()
{
    disconnect( m_controlView->marbleWidget(), SIGNAL(pluginSettingsChanged()),
                this,                          SLOT(writePluginSettings()) );

    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig();

    foreach( RenderPlugin *plugin, m_controlView->marbleWidget()->renderPlugins() ) {
        KConfigGroup group = sharedConfig->group( QString( "plugin_" ) + plugin->nameId() );

        QHash<QString,QVariant> hash;

        foreach ( const QString& key, group.keyList() ) {
            hash.insert( key, group.readEntry( key ) );
        }

        plugin->setSettings( hash );
    }

    connect( m_controlView->marbleWidget(), SIGNAL(pluginSettingsChanged()),
             this,                          SLOT(writePluginSettings()) );
}

void MarblePart::lockFloatItemPosition( bool enabled )
{
    QList<AbstractFloatItem *> floatItemList = m_controlView->marbleWidget()->floatItems();

    QList<AbstractFloatItem *>::const_iterator i = floatItemList.constBegin();
    QList<AbstractFloatItem *>::const_iterator const end = floatItemList.constEnd();
    for (; i != end; ++i ) {
        // Locking one would suffice as it affects all.
        // Nevertheless go through all.
        (*i)->setPositionLocked(enabled);
    }
}

void MarblePart::handleProgress( int active, int queued )
{
    m_downloadProgressBar->setUpdatesEnabled( false );
    if ( m_downloadProgressBar->value() < 0 ) {
        m_downloadProgressBar->setMaximum( 1 );
        m_downloadProgressBar->setValue( 0 );
        m_downloadProgressBar->setVisible( MarbleSettings::showDownloadProgressBar() );
    } else {
        m_downloadProgressBar->setMaximum( qMax<int>( m_downloadProgressBar->maximum(), active + queued ) );
    }

//     qDebug() << "downloadProgressJobAdded: value/maximum: "
//              << m_downloadProgressBar->value() << '/' << m_downloadProgressBar->maximum();

    m_downloadProgressBar->setUpdatesEnabled( true );
}

void MarblePart::removeProgressItem()
{
    m_downloadProgressBar->setUpdatesEnabled( false );
    m_downloadProgressBar->setValue( m_downloadProgressBar->value() + 1 );
    if ( m_downloadProgressBar->value() == m_downloadProgressBar->maximum() ) {
        m_downloadProgressBar->reset();
        m_downloadProgressBar->setVisible( false );
    }

//     qDebug() << "downloadProgressJobCompleted: value/maximum: "
//              << m_downloadProgressBar->value() << '/' << m_downloadProgressBar->maximum();

    m_downloadProgressBar->setUpdatesEnabled( true );
}

void MarblePart::openEditBookmarkDialog()
{
    MarbleWidget *widget = m_controlView->marbleWidget();
    QPointer<EditBookmarkDialog> dialog = new EditBookmarkDialog( widget->model()->bookmarkManager(), widget );
    dialog->setCoordinates( widget->lookAt().coordinates() );
    dialog->setRange( widget->lookAt().range() );
    dialog->setMarbleWidget( widget );
    dialog->setReverseGeocodeName();
    if ( dialog->exec() == QDialog::Accepted ) {
        widget->model()->bookmarkManager()->addBookmark( dialog->folder(), dialog->bookmark() );
    }
    delete dialog;
}

void MarblePart::setHome()
{
    MarbleWidget *widget = m_controlView->marbleWidget();
    widget->model()->setHome( widget->centerLongitude(), widget->centerLatitude(), widget->zoom() );
}

void MarblePart::openManageBookmarksDialog()
{
    MarbleModel * const model = m_controlView->marbleModel();
    QPointer<BookmarkManagerDialog> dialog = new BookmarkManagerDialog( model, m_controlView->marbleWidget() );
    dialog->exec();
    delete dialog;
}

void MarblePart::lookAtBookmark( QAction *action)
{
        GeoDataLookAt temp = qvariant_cast<GeoDataLookAt>( action->data() ) ;
        m_controlView->marbleWidget()->flyTo( temp ) ;
                mDebug() << " looking at bookmark having longitude : "<< temp.longitude(GeoDataCoordinates::Degree)
                         << " latitude :  "<< temp.latitude(GeoDataCoordinates::Degree)
                         << " distance : " << temp.range();
}


void MarblePart::initializeCustomTimezone()
{
    if( m_timezone.count() == 0)
    {
        m_timezone.insert( 0, 0 );
        m_timezone.insert( 1, 3600 );
        m_timezone.insert( 2, 7200 );
        m_timezone.insert( 3, 7200 );
        m_timezone.insert( 4, 10800 );
        m_timezone.insert( 5, 12600 );
        m_timezone.insert( 6, 14400 );
        m_timezone.insert( 7, 18000 );
        m_timezone.insert( 8, 19800 );
        m_timezone.insert( 9, 21600 );
        m_timezone.insert( 10, 25200 );
        m_timezone.insert( 11, 28800 );
        m_timezone.insert( 12, 32400 );
        m_timezone.insert( 13, 34200 );
        m_timezone.insert( 14, 36000 );
        m_timezone.insert( 15, 39600 );
        m_timezone.insert( 16, 43200 );
        m_timezone.insert( 17, -39600 );
        m_timezone.insert( 18, -36000 );
        m_timezone.insert( 19, -32400 );
        m_timezone.insert( 20, -28800 );
        m_timezone.insert( 21, -25200 );
        m_timezone.insert( 22, -25200 );
        m_timezone.insert( 23, -21600 );
        m_timezone.insert( 24, -18000 );
        m_timezone.insert( 25, -18000 );
        m_timezone.insert( 26, -14400 );
        m_timezone.insert( 27, -12600 );
        m_timezone.insert( 28, -10800 );
        m_timezone.insert( 29, -10800 );
        m_timezone.insert( 30, -3600 );
    }
}

void MarblePart::printMapScreenShot()
{
#ifndef QT_NO_PRINTER
    QPrinter printer( QPrinter::HighResolution );
    QPointer<QPrintDialog> printDialog = new QPrintDialog( &printer, widget() );
    m_controlView->printMapScreenShot( printDialog );
    delete printDialog;
#endif
}

void MarblePart::updateMapEditButtonVisibility( const QString &mapTheme )
{
    Q_ASSERT( m_externalMapEditorAction );
    m_externalMapEditorAction->setVisible(mapTheme == QLatin1String("earth/openstreetmap/openstreetmap.dgml"));
}

void MarblePart::fallBackToDefaultTheme()
{
    m_controlView->marbleWidget()->setMapThemeId( m_controlView->defaultMapThemeId() );
}

void MarblePart::showMovieCaptureDialog()
{
    if (m_movieCaptureDialog == 0) {
        m_movieCaptureDialog = new MovieCaptureDialog(m_controlView->marbleWidget(),
                                                      m_controlView->marbleWidget());
        connect( m_movieCaptureDialog, SIGNAL(started()), this, SLOT(changeRecordingState()));
    }
    m_movieCaptureDialog->show();
}

void MarblePart::stopRecording()
{
    if ( m_movieCaptureDialog ) {
        m_movieCaptureDialog->stopRecording();
        changeRecordingState();
    }
}

void MarblePart::changeRecordingState()
{
    m_recordMovieAction->setEnabled( !m_recordMovieAction->isEnabled() );
    m_stopRecordingAction->setEnabled( !m_stopRecordingAction->isEnabled() );
}

}

#include "moc_marble_part.cpp"
#include "marble_part.moc"
