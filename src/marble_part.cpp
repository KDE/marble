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
#include <QtCore/QDir>
#include <QtCore/QPointer>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtGui/QClipboard>
#include <QtGui/QLabel>
#include <QtGui/QFontMetrics>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QProgressBar>
#include <QtGui/QStandardItemModel>
#include <QtGui/QShortcut>
#include <QtNetwork/QNetworkProxy>

// KDE
#include <kaboutdata.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kcomponentdata.h>
#include <kconfigdialog.h>
#include <kdeversion.h>
#include <kfiledialog.h>
#include <kicon.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpluginfactory.h>
#include <kparts/statusbarextension.h>
#include <kstandardaction.h>
#include <kstatusbar.h>
#include <ktoggleaction.h>
#include <ktogglefullscreenaction.h>
#include <knewstuff3/knewstuffaction.h>
#include <knewstuff3/downloaddialog.h>
#include <knewstuff3/uploaddialog.h>
#include <KStandardDirs>
#include <kdeprintdialog.h>
#include <KToolBar>

// Marble library classes
#include "AbstractFloatItem.h"
#include "AbstractDataPlugin.h"
#include "EditBookmarkDialog.h"
#include "BookmarkManager.h"
#include "BookmarkManagerDialog.h"
#include "DialogConfigurationInterface.h"
#include "DownloadRegionDialog.h"
#include "GeoDataCoordinates.h"
#include "GeoDataFolder.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLookAt.h"
#include "GeoDataPlacemark.h"
#include "HttpDownloadManager.h"
#include "MarbleCacheSettingsWidget.h"
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

// Marble non-library classes
#include "ControlView.h"
#include "settings.h"

using namespace Marble;

#include "ui_MarbleViewSettingsWidget.h"
#include "ui_MarbleNavigationSettingsWidget.h"
#include "ui_MarbleTimeSettingsWidget.h"

namespace Marble
{

namespace
{
    const char* POSITION_STRING = I18N_NOOP( "Position: %1" );
    const char* DISTANCE_STRING = I18N_NOOP( "Altitude: %1" );
    const char* TILEZOOMLEVEL_STRING = I18N_NOOP( "Tile Zoom Level: %1" );
    const char* DATETIME_STRING = I18N_NOOP( "Time: %1" );
}

K_PLUGIN_FACTORY(MarblePartFactory, registerPlugin<MarblePart>();)
K_EXPORT_PLUGIN(MarblePartFactory("marble"))

MarblePart::MarblePart( QWidget *parentWidget, QObject *parent, const QVariantList &arguments )
  : KParts::ReadOnlyPart( parent ),
    m_sunControlDialog( 0 ),
    m_timeControlDialog( 0 ),
    m_downloadRegionDialog( 0 ),
    m_externalMapEditorAction( 0 ),
    m_pluginModel( 0 ),
    m_configDialog( 0 ),
    m_position( i18n( NOT_AVAILABLE ) ),
    m_tileZoomLevel( i18n( NOT_AVAILABLE ) ),
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
    KLocale *kLocale = KGlobal::locale();
    if ( kLocale->measureSystem() == KLocale::Metric ) {
        marbleLocale->setMeasurementSystem( QLocale::MetricSystem );
    }
    else {
        marbleLocale->setMeasurementSystem( QLocale::ImperialSystem );
    }

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

    connect( m_controlView, SIGNAL( showUploadDialog() ), this, SLOT( showUploadNewStuffDialog() ) );
    connect( m_controlView, SIGNAL( showMapWizard() ), this, SLOT( showMapWizard() ) );
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
    return new KAboutData( I18N_NOOP( "marble_part" ), 0,
                           ki18n( "A Virtual Globe" ),
                           ControlView::applicationVersion().toLatin1() );
}

bool MarblePart::openUrl( const KUrl &url )
{
    Q_UNUSED( url );

    return true;
}

bool MarblePart::openFile()
{
    const PluginManager *const pluginManager = m_controlView->marbleModel()->pluginManager();

    QStringList allFileExtensions;
    QStringList filters;
    foreach ( const ParseRunnerPlugin *plugin, pluginManager->parsingRunnerPlugins() ) {
        if ( plugin->nameId() == "Cache" )
            continue;

        const QStringList fileExtensions = plugin->fileExtensions().replaceInStrings( QRegExp( "^" ), "*." );
        const QString filter = QString( "%1|%2" ).arg( fileExtensions.join( " " ) ).arg( plugin->fileFormatDescription() );
        filters << filter;

        allFileExtensions << fileExtensions;
    }

    allFileExtensions.sort();
    const QString allFileTypes = QString( "%1|%2" ).arg( allFileExtensions.join( " " ) ).arg( i18n( "All Supported Files" ) );

    filters.sort();
    filters.prepend( allFileTypes );
    const QString filter = filters.join( "\n" );

    QStringList fileNames = KFileDialog::getOpenFileNames( m_lastFileOpenPath, filter,
                                            widget(), i18n("Open File")
                                           );

    if ( !fileNames.isEmpty() ) {
        const QString firstFile = fileNames.first();
        m_lastFileOpenPath = KUrl::fromLocalFile( QFileInfo( firstFile ).absolutePath() );
    }

    foreach( const QString &fileName, fileNames ) {
        m_controlView->marbleModel()->addGeoDataFile( fileName );
    }

    return true;
}


void MarblePart::exportMapScreenShot()
{
    QString  fileName = KFileDialog::getSaveFileName( QDir::homePath(),
                                                      i18n( "Images *.jpg *.png" ),
                                                      widget(), i18n("Export Map") );

    if ( !fileName.isEmpty() ) {
        // Take the case into account where no file format is indicated
        const char * format = 0;
        if ( !fileName.endsWith("png", Qt::CaseInsensitive)
           && !fileName.endsWith("jpg", Qt::CaseInsensitive) )
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

void MarblePart::setShowAtmosphere( bool isChecked )
{
    m_controlView->marbleWidget()->setShowAtmosphere( isChecked );

    m_showAtmosphereAction->setChecked( isChecked ); // Sync state with the GUI
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
    if ( KApplication::activeWindow() )
        KToggleFullScreenAction::setFullScreen( KApplication::activeWindow(), isChecked );

    m_fullScreenAct->setChecked( isChecked ); // Sync state with the GUI
}

void MarblePart::showSideBar( bool isChecked )
{
    m_controlView->setSideBarShown( isChecked );

    m_sideBarAct->setChecked( isChecked ); // Sync state with the GUI
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
        connect( m_sunControlDialog, SIGNAL( showSun( bool ) ),
                 this,               SLOT ( showSun( bool ) ) );
        connect( m_sunControlDialog, SIGNAL( showSun( bool ) ),
                 m_showShadow,               SLOT ( setChecked( bool ) ) );
        connect( m_sunControlDialog, SIGNAL( showSunInZenith( bool ) ),
                 m_showSunInZenith,               SLOT ( setChecked( bool ) ) );
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

void MarblePart::showSunInZenith( bool active )
{
    m_controlView->marbleWidget()->setShowSunInZenith( active );
}

void MarblePart::workOffline( bool offline )
{
    m_controlView->marbleControl()->setWorkOffline( offline );
    m_newStuffAction->setEnabled( !offline );
    m_downloadRegionAction->setEnabled( !offline );
}

void MarblePart::copyMap()
{
    QPixmap      mapPixmap = m_controlView->mapScreenShot();
    QClipboard  *clipboard = KApplication::clipboard();

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

void MarblePart::setShowCurrentLocation( bool show )
{
    m_controlView->setCurrentLocationTabShown( show );
}

void MarblePart::readSettings()
{
    kDebug() << "Start: MarblePart::readSettings()";
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

    setShowCurrentLocation( MarbleSettings::showCurrentLocation() );
    m_currentLocationAction->setChecked( MarbleSettings::showCurrentLocation() );

    m_controlView->marbleWidget()->setShowAtmosphere( MarbleSettings::showAtmosphere() );
    m_showAtmosphereAction->setChecked( MarbleSettings::showAtmosphere() );
    m_lockFloatItemsAct->setChecked(MarbleSettings::lockFloatItemPositions());
    lockFloatItemPosition(MarbleSettings::lockFloatItemPositions());
    m_controlView->marbleWidget()->inputHandler()->setKineticScrollingEnabled( MarbleSettings::kineticScrolling() );

    setShowBookmarks( MarbleSettings::showBookmarks() );

    // Sun
    m_controlView->marbleWidget()->setShowSunShading( MarbleSettings::showSun() );
    m_showShadow->setChecked( MarbleSettings::showSun() );
    m_controlView->marbleWidget()->setShowCityLights( MarbleSettings::showCitylights() );
    m_controlView->marbleWidget()->setShowSunInZenith( MarbleSettings::centerOnSun() );
    m_showSunInZenith->setChecked( MarbleSettings::centerOnSun() );

    // View
    m_initialGraphicsSystem = (GraphicsSystem) MarbleSettings::graphicsSystem();
    m_previousGraphicsSystem = m_initialGraphicsSystem;

    m_lastFileOpenPath = KUrl::fromLocalFile( MarbleSettings::lastFileOpenDir() );

    // Plugins
    QHash<QString, int> pluginEnabled;
    QHash<QString, int> pluginVisible;

    int nameIdSize = MarbleSettings::pluginNameId().size();
    int enabledSize = MarbleSettings::pluginEnabled().size();
    int visibleSize = MarbleSettings::pluginVisible().size();

    if ( nameIdSize == enabledSize ) {
        for ( int i = 0; i < enabledSize; ++i ) {
            pluginEnabled[ MarbleSettings::pluginNameId()[i] ]
                = MarbleSettings::pluginEnabled()[i];
        }
    }

    if ( nameIdSize == visibleSize ) {
        for ( int i = 0; i < visibleSize; ++i ) {
            pluginVisible[ MarbleSettings::pluginNameId()[i] ]
                = MarbleSettings::pluginVisible()[i];
        }
    }

    QList<RenderPlugin *> pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        if ( pluginEnabled.contains( (*i)->nameId() ) ) {
            (*i)->setEnabled( pluginEnabled[ (*i)->nameId() ] );
            // I think this isn't needed, as it is part of setEnabled()
//             (*i)->item()->setCheckState( pluginEnabled[ (*i)->nameId() ]  ?  Qt::Checked : Qt::Unchecked );
        }
        if ( pluginVisible.contains( (*i)->nameId() ) ) {
            (*i)->setVisible( pluginVisible[ (*i)->nameId() ] );
        }
    }

    // Load previous route settings
    m_controlView->marbleModel()->routingManager()->readSettings();
    bool const startupWarning = MarbleSettings::showGuidanceModeStartupWarning();
    m_controlView->marbleModel()->routingManager()->setShowGuidanceModeStartupWarning( startupWarning );

    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig( KGlobal::mainComponent() );
    if ( sharedConfig->hasGroup( "Routing Profiles" ) ) {
        QList<RoutingProfile> profiles;
        KConfigGroup profilesGroup = sharedConfig->group( "Routing Profiles" );
        int numProfiles = profilesGroup.readEntry( "Num", 0 );
        for ( int i = 0; i < numProfiles; ++i ) {
            KConfigGroup profileGroup = profilesGroup.group( QString( "Profile %0" ).arg(i) );
            QString name = profileGroup.readEntry( "Name", tr( "Unnamed" ) );
            RoutingProfile profile( name );
            foreach ( const QString& pluginName, profileGroup.groupList() ) {
                KConfigGroup pluginGroup = profileGroup.group( pluginName );
                profile.pluginSettings().insert( pluginName, QHash<QString, QVariant>() );
                foreach ( const QString& key, pluginGroup.keyList() ) {
                    if ( key != "Enabled" ) {
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

    QString positionProvider = MarbleSettings::activePositionTrackingPlugin();
    if ( !positionProvider.isEmpty() ) {
        PositionTracking* tracking = m_controlView->marbleModel()->positionTracking();
        const PluginManager* pluginManager = m_controlView->marbleModel()->pluginManager();
        foreach( const PositionProviderPlugin* plugin, pluginManager->positionProviderPlugins() ) {
            if ( plugin->nameId() == positionProvider ) {
                PositionProviderPlugin* instance = plugin->newInstance();
                instance->setMarbleModel( m_controlView->marbleModel() );
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

    disconnect( m_controlView->marbleWidget(), SIGNAL( pluginSettingsChanged() ),
                this,                          SLOT( writePluginSettings() ) );
    connect( m_controlView->marbleWidget(), SIGNAL( pluginSettingsChanged() ),
             this,                          SLOT( writePluginSettings() ) );

    m_controlView->setExternalMapEditor( m_externalEditorMapping[MarbleSettings::externalMapEditor()] );
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

    m_controlView->marbleModel()->home( homeLon, homeLat, homeZoom );

    // Map theme and projection
    MarbleSettings::setMapTheme( m_controlView->marbleWidget()->mapThemeId() );
    MarbleSettings::setProjection( m_controlView->marbleWidget()->projection() );

    MarbleSettings::setShowClouds( m_controlView->marbleWidget()->showClouds() );

    MarbleSettings::setWorkOffline( m_workOfflineAction->isChecked() );
    MarbleSettings::setShowAtmosphere( m_controlView->marbleWidget()->showAtmosphere() );

    MarbleSettings::setShowCurrentLocation( m_currentLocationAction->isChecked() );

    MarbleSettings::setStillQuality( m_controlView->marbleWidget()->mapQuality( Still ) );
    MarbleSettings::setAnimationQuality( m_controlView->marbleWidget()->
                                         mapQuality( Animation ) );

    MarbleSettings::setShowBookmarks( m_controlView->marbleModel()->bookmarkManager()->showBookmarks() );

    // FIXME: Hopefully Qt will have a getter for this one in the future ...
    GraphicsSystem graphicsSystem = (GraphicsSystem) MarbleSettings::graphicsSystem();
    MarbleSettings::setGraphicsSystem( graphicsSystem );

    MarbleSettings::setLastFileOpenDir( m_lastFileOpenPath.toLocalFile() );

    MarbleSettings::setDistanceUnit( MarbleGlobal::getInstance()->locale()->measurementSystem() );
    MarbleSettings::setAngleUnit( m_controlView->marbleWidget()->defaultAngleUnit() );

    // Sun
    MarbleSettings::setShowSun( m_controlView->marbleWidget()->showSunShading() );
    MarbleSettings::setShowCitylights( m_controlView->marbleWidget()->showCityLights() );
    MarbleSettings::setCenterOnSun( m_controlView->marbleWidget()->showSunInZenith() );

    // Caches
    MarbleSettings::setVolatileTileCacheLimit( m_controlView->marbleWidget()->
                                               volatileTileCacheLimit() / 1024 );
    MarbleSettings::setPersistentTileCacheLimit( m_controlView->marbleModel()->
                                                 persistentTileCacheLimit() / 1024 );

    // Time
    MarbleSettings::setDateTime( m_controlView->marbleModel()->clockDateTime() );
    MarbleSettings::setSpeedSlider( m_controlView->marbleModel()->clockSpeed() );

    // Plugins
    QList<int>   pluginEnabled;
    QList<int>   pluginVisible;
    QStringList  pluginNameId;

    QList<RenderPlugin *> pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        pluginEnabled << static_cast<int>( (*i)->enabled() );
        pluginVisible << static_cast<int>( (*i)->visible() );
        pluginNameId  << (*i)->nameId();
    }
    MarbleSettings::setPluginEnabled( pluginEnabled );
    MarbleSettings::setPluginVisible( pluginVisible );
    MarbleSettings::setPluginNameId(  pluginNameId );

    QString positionProvider;
    PositionTracking* tracking = m_controlView->marbleModel()->positionTracking();
    if ( tracking && tracking->positionProviderPlugin() ) {
        positionProvider = tracking->positionProviderPlugin()->nameId();
    }
    MarbleSettings::setActivePositionTrackingPlugin( positionProvider );

    MarbleSettings::setLockFloatItemPositions( m_lockFloatItemsAct->isChecked() );

    writeStatusBarSettings();

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

    MarbleSettings::self()->writeConfig();
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
    // Action: Download Region
    m_downloadRegionAction = new KAction( this );
    m_downloadRegionAction->setText( i18nc( "Action for downloading an entire region of a map",
                                            "Download Region..." ));
    actionCollection()->addAction( "file_download_region", m_downloadRegionAction );
    connect( m_downloadRegionAction, SIGNAL( triggered() ), SLOT( showDownloadRegionDialog() ));

    // Action: Print Map
    m_printMapAction = KStandardAction::print( this, SLOT( printMapScreenShot() ),
                                               actionCollection() );

    m_printPreviewAction = KStandardAction::printPreview( m_controlView, SLOT( printPreview() ),
                                               actionCollection() );

    // Action: Export Map
    m_exportMapAction = new KAction( this );
    actionCollection()->addAction( "exportMap", m_exportMapAction );
    m_exportMapAction->setText( i18nc( "Action for saving the map to a file", "&Export Map..." ) );
    m_exportMapAction->setIcon( KIcon( "document-save-as" ) );
    m_exportMapAction->setShortcut( Qt::CTRL + Qt::Key_S );
    connect( m_exportMapAction, SIGNAL(triggered( bool ) ),
             this,              SLOT( exportMapScreenShot() ) );

    // Action: Work Offline
    m_workOfflineAction = new KAction( this );
    actionCollection()->addAction( "workOffline", m_workOfflineAction );
    m_workOfflineAction->setText( i18nc( "Action for toggling offline mode", "&Work Offline" ) );
    m_workOfflineAction->setIcon( KIcon( "user-offline" ) );
    m_workOfflineAction->setCheckable( true );
    m_workOfflineAction->setChecked( false );
    connect( m_workOfflineAction, SIGNAL( triggered( bool ) ),
             this,                SLOT( workOffline( bool ) ) );

    // Action: Current Location
    m_currentLocationAction = new KAction( this );
    actionCollection()->addAction( "show_currentlocation", m_currentLocationAction );
    m_currentLocationAction->setText( i18nc( "Action for toggling the 'current location' box",
                                             "Current Location" ) );
    m_currentLocationAction->setCheckable( true );
    m_currentLocationAction->setChecked( false );
    connect( m_currentLocationAction, SIGNAL( triggered( bool ) ),
             this,                SLOT( setShowCurrentLocation( bool ) ) );

    // Action: Copy Map to the Clipboard
    m_copyMapAction = KStandardAction::copy( this, SLOT( copyMap() ),
                                             actionCollection() );
    m_copyMapAction->setText( i18nc( "Action for copying the map to the clipboard", "&Copy Map" ) );

    // Action: Copy Coordinates string
    m_copyCoordinatesAction = new KAction( this );
    actionCollection()->addAction( "edit_copy_coordinates",
                                   m_copyCoordinatesAction );
    m_copyCoordinatesAction->setText( i18nc( "Action for copying the coordinates to the clipboard",
                                             "C&opy Coordinates" ) );
    connect( m_copyCoordinatesAction, SIGNAL( triggered( bool ) ),
             this,                    SLOT( copyCoordinates() ) );

    // Action: Open a Gpx or a Kml File
    m_openAct = KStandardAction::open( this, SLOT( openFile() ),
                                       actionCollection() );
    m_openAct->setText( i18nc( "Action for opening a file", "&Open..." ) );

    // Standard actions.  So far only Quit.
    KStandardAction::quit( kapp, SLOT( closeAllWindows() ),
                           actionCollection() );

    // Action: Get hot new stuff
    m_newStuffAction = KNS3::standardAction( i18nc( "Action for downloading maps (GHNS)",
                                                    "Download Maps..."),
                                             this,
                                             SLOT( showNewStuffDialog() ),
                                             actionCollection(), "new_stuff" );
    m_newStuffAction->setStatusTip( i18nc( "Status tip", "Download new maps"));
    m_newStuffAction->setShortcut( Qt::CTRL + Qt::Key_N );

    // Action: Create a New Map
    m_mapWizardAct = new KAction( i18nc( "Action for creating new maps",
                                         "&Create a New Map..." ),
                                  this );
    actionCollection()->addAction( "createMap", m_mapWizardAct );
    m_mapWizardAct->setStatusTip( i18nc( "Status tip",
                                         "A wizard guides you through the creation of your own map theme." ) );
    connect( m_mapWizardAct, SIGNAL( triggered() ), SLOT( showMapWizard() ) );

    KStandardAction::showStatusbar( this, SLOT( showStatusBar( bool ) ),
                                    actionCollection() );

    m_sideBarAct = new KAction( i18nc( "Action for toggling the navigation panel",
                                       "Show &Navigation Panel"), this );
    actionCollection()->addAction( "options_show_sidebar", m_sideBarAct );
    m_sideBarAct->setShortcut( Qt::Key_F9 );
    m_sideBarAct->setCheckable( true );
    m_sideBarAct->setChecked( true );
    m_sideBarAct->setStatusTip( i18nc( "Status tip", "Show Navigation Panel" ) );
    connect( m_sideBarAct, SIGNAL( triggered( bool ) ),
             this,         SLOT( showSideBar( bool ) ) );

    m_fullScreenAct = KStandardAction::fullScreen( 0, 0, widget(),
                                                   actionCollection() );
    connect( m_fullScreenAct, SIGNAL( triggered( bool ) ),
             this,            SLOT( showFullScreen( bool ) ) );

    // Action: Show Atmosphere option
    m_showAtmosphereAction = new KAction( this );
    actionCollection()->addAction( "show_atmosphere", m_showAtmosphereAction );
    m_showAtmosphereAction->setCheckable( true );
    m_showAtmosphereAction->setVisible( false );
    m_showAtmosphereAction->setChecked( true );
    m_showAtmosphereAction->setText( i18nc( "Action for toggling the atmosphere", "&Atmosphere" ) );
    connect( m_showAtmosphereAction, SIGNAL( triggered( bool ) ),
             this,                   SLOT( setShowAtmosphere( bool ) ) );
    foreach ( RenderPlugin *plugin, m_controlView->marbleWidget()->renderPlugins() ) {
        if ( plugin->nameId() == "atmosphere" ) {
            m_showAtmosphereAction->setVisible( plugin->enabled() );
            connect( plugin, SIGNAL( enabledChanged( bool ) ),
                     m_showAtmosphereAction, SLOT( setVisible( bool ) ) );
        }
    }

    // Action: Show Crosshairs option
    QList<RenderPlugin *> pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        if ( (*i)->nameId() == "crosshairs" ) {
            actionCollection()->addAction( "show_crosshairs", (*i)->action() );
        }
    }


    // Action: Show Clouds option
    m_showCloudsAction = new KAction( this );
    actionCollection()->addAction( "show_clouds", m_showCloudsAction );
    m_showCloudsAction->setCheckable( true );
    m_showCloudsAction->setChecked( true );
    m_showCloudsAction->setText( i18nc( "Action for toggling clouds", "&Clouds" ) );
    connect( m_showCloudsAction, SIGNAL( triggered( bool ) ),
             this,               SLOT( setShowClouds( bool ) ) );

    // Action: Show Sunshade options
    m_controlSunAction = new KAction( this );
    actionCollection()->addAction( "control_sun", m_controlSunAction );
    m_controlSunAction->setText( i18nc( "Action for sun control dialog", "S&un Control..." ) );
    connect( m_controlSunAction, SIGNAL( triggered( bool ) ),
             this,               SLOT( controlSun() ) );

    KStandardAction::redisplay( m_controlView->marbleWidget(), SLOT( reloadMap() ), actionCollection() );

    // Action: Show Time options
    m_controlTimeAction = new KAction( this );
    actionCollection()->addAction( "control_time", m_controlTimeAction );
    m_controlTimeAction->setText( i18nc( "Action for time control dialog", "&Time Control..." ) );
    connect( m_controlTimeAction, SIGNAL( triggered( bool ) ),
         this,               SLOT( controlTime() ) );

    // Action: Lock float items
    m_lockFloatItemsAct = new KAction ( this );
    actionCollection()->addAction( "options_lock_floatitems",
                                   m_lockFloatItemsAct );
    m_lockFloatItemsAct->setText( i18nc( "Action for locking float items on the map",
                                         "Lock Position" ) );
    m_lockFloatItemsAct->setCheckable( true );
    m_lockFloatItemsAct->setChecked( false );
    connect( m_lockFloatItemsAct, SIGNAL( triggered( bool ) ),
             this,                SLOT( lockFloatItemPosition( bool ) ) );

    KStandardAction::preferences( this, SLOT( editSettings() ),
                                  actionCollection() );

    //Toggle Action: Show sun shadow
    m_showShadow = new KToggleAction( i18n( "Show Shadow" ), this );
    m_showShadow->setIcon( KIcon( "" ) );        // Fixme: Add Icon
    actionCollection()->addAction( "sun_shadow", m_showShadow );
    m_showShadow->setCheckedState( KGuiItem( i18n( "Hide Shadow" ) ) );
    m_showShadow->setToolTip(i18n("Shows and hides the shadow of the sun"));
    connect( m_showShadow, SIGNAL( triggered( bool ) ), this, SLOT( showSun( bool ) ));

    //Toggle Action: Show sun zenith
    m_showSunInZenith = new KToggleAction( i18n( "Show Zenith" ), this );
    m_showSunInZenith->setIcon( KIcon( MarbleDirs::path( "svg/sunshine.png" ) ) );
    actionCollection()->addAction( "sun_zenith", m_showSunInZenith );
    m_showSunInZenith->setCheckedState( KGuiItem( i18n( "Hide Zenith" ) ) );
    m_showSunInZenith->setToolTip( i18n( "Shows and hides the zenith location of the sun" ) );
    connect( m_showSunInZenith, SIGNAL( triggered( bool ) ), this, SLOT( showSunInZenith( bool ) ));

    //    FIXME: Discuss if this is the best place to put this
    QList<RenderPlugin *>::const_iterator it = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const itEnd = pluginList.constEnd();
    for (; it != itEnd; ++it ) {
        connect( (*it), SIGNAL( actionGroupsChanged() ),
                 this, SLOT( createPluginMenus() ) );
    }

    m_addBookmarkAction = new KAction( this );
    actionCollection()->addAction( "add_bookmark", m_addBookmarkAction );
    m_addBookmarkAction->setText( i18nc( "Add Bookmark", "&Add Bookmark" ) );
    m_addBookmarkAction->setIcon( KIcon( ":/icons/bookmark-new.png" ) );
    m_addBookmarkAction->setShortcut( Qt::CTRL + Qt::Key_B );
    connect( m_addBookmarkAction, SIGNAL( triggered( ) ),
             this,                SLOT( openEditBookmarkDialog() ) );

    m_toggleBookmarkDisplayAction = new KAction( this );
    actionCollection()->addAction( "show_bookmarks", m_toggleBookmarkDisplayAction );
    m_toggleBookmarkDisplayAction->setText( i18nc( "Show Bookmarks", "Show &Bookmarks" ) );
    m_toggleBookmarkDisplayAction->setStatusTip( tr( "Show or hide bookmarks in the map" ) );
    m_toggleBookmarkDisplayAction->setCheckable( true );
    m_toggleBookmarkDisplayAction->setChecked( m_controlView->marbleModel()->bookmarkManager()->showBookmarks() );
    connect( m_toggleBookmarkDisplayAction, SIGNAL( toggled( bool ) ),
                  m_controlView->marbleModel()->bookmarkManager(), SLOT( setShowBookmarks( bool ) ) );

    m_setHomeAction = new KAction( this );
    actionCollection()->addAction( "set_home", m_setHomeAction );
    m_setHomeAction->setText( tr( "&Set Home Location" ) );
    m_setHomeAction->setIcon( KIcon( "go-home" ) );
    connect( m_setHomeAction, SIGNAL( triggered( ) ),
             this,                SLOT( setHome() ) );

    m_manageBookmarksAction = new KAction( this );
    actionCollection()->addAction( "manage_bookmarks", m_manageBookmarksAction );
    m_manageBookmarksAction->setText( i18nc( "Manage Bookmarks", "&Manage Bookmarks" ) );
    m_manageBookmarksAction->setIcon( KIcon( ":/icons/bookmarks-organize.png" ) );
    connect( m_manageBookmarksAction, SIGNAL( triggered( ) ),
             this,                SLOT( openManageBookmarksDialog() ) );


    createFolderList();
    connect( m_controlView->marbleModel()->bookmarkManager(),
             SIGNAL( bookmarksChanged() ), this, SLOT( createFolderList() ) );

    m_externalMapEditorAction = new KAction( this );
    actionCollection()->addAction( "external_editor", m_externalMapEditorAction );
    m_externalMapEditorAction->setText( i18nc( "Edit the map in an external application", "&Edit Map" ) );
    m_externalMapEditorAction->setShortcut( Qt::CTRL + Qt::Key_E );
    connect( m_externalMapEditorAction, SIGNAL( triggered( ) ),
             m_controlView, SLOT( launchExternalMapEditor() ) );
    connect( m_controlView->marbleWidget(), SIGNAL( themeChanged( QString ) ),
             this, SLOT( updateMapEditButtonVisibility( QString ) ) );
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
       connect( m_bookmarksListMenu, SIGNAL( triggered ( QAction *) ),
            this, SLOT( lookAtBookmark( QAction *) ) );

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

        if( (*i)->renderType() == RenderPlugin::Online ) {
            actionList.append( (*i)->action() );
        }
    }

    unplugActionList( "onlineservices_actionlist" );
    plugActionList( "onlineservices_actionlist", actionList );
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
        m_tileZoomLevel = i18n( NOT_AVAILABLE );
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
    QList<QActionGroup*> *tmp_toolbarActionGroups;
    QList<RenderPlugin *> renderPluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = renderPluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = renderPluginList.constEnd();

    //Load the toolbars
    for (; i != end; ++i ) {
        tmp_toolbarActionGroups = (*i)->toolbarActionGroups();

        if ( tmp_toolbarActionGroups ) {

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
        m_tileZoomLevel = i18n( NOT_AVAILABLE );
    else {
        m_tileZoomLevel.setNum( tileZoomLevel );
    }
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

    QString templatePositionString =
        QString( "%1 000\xb0 00\' 00\"_, 000\xb0 00\' 00\"_" ).arg(POSITION_STRING);
    m_positionLabel = setupStatusBarLabel( templatePositionString );

    QString templateDistanceString =
        QString( "%1 00.000,0 mu" ).arg(DISTANCE_STRING);
    m_distanceLabel = setupStatusBarLabel( templateDistanceString );

    QString templateDateTimeString = QString( "%1 %2" ).arg( DATETIME_STRING , QLocale().toString( QDateTime::fromString ( "01:01:1000", "dd:mm:yyyy"), QLocale::ShortFormat ) );

    m_clockLabel = setupStatusBarLabel( templateDateTimeString );

    const QString templateTileZoomLevelString = i18n( TILEZOOMLEVEL_STRING );
    m_tileZoomLevelLabel = setupStatusBarLabel( templateTileZoomLevelString );

    connect( m_controlView->marbleWidget(), SIGNAL( mouseMoveGeoPosition( QString ) ),
             this,                          SLOT( showPosition( QString ) ) );
    connect( m_controlView->marbleWidget(), SIGNAL( distanceChanged( QString ) ),
             this,                          SLOT( updateStatusBar() ) );
    connect( m_controlView->marbleWidget(), SIGNAL( tileLevelChanged( int )),
             SLOT( showZoomLevel( int )));
    connect( m_controlView->marbleWidget(), SIGNAL( themeChanged( QString )),
             this, SLOT( mapThemeChanged( QString )), Qt::QueuedConnection );
    connect( m_controlView->marbleModel()->clock(), SIGNAL( timeChanged() ),
             this,                          SLOT( showDateTime() ) );


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
    KStatusBar * const statusBar = m_statusBarExtension->statusBar();
    Q_ASSERT( statusBar );

    m_downloadProgressBar = new QProgressBar;
    m_downloadProgressBar->setVisible( MarbleSettings::showDownloadProgressBar() );
    statusBar->addPermanentWidget( m_downloadProgressBar );

    HttpDownloadManager * const downloadManager =
        m_controlView->marbleModel()->downloadManager();
    Q_ASSERT( downloadManager );
    connect( downloadManager, SIGNAL( jobAdded() ), SLOT( downloadJobAdded() ) );
    connect( downloadManager, SIGNAL( jobRemoved() ), SLOT( downloadJobRemoved() ) );
}

void MarblePart::setupStatusBarActions()
{
    KStatusBar * const statusBar = m_statusBarExtension->statusBar();
    Q_ASSERT( statusBar );

    statusBar->setContextMenuPolicy( Qt::CustomContextMenu );

    connect( statusBar, SIGNAL( customContextMenuRequested( QPoint )),
             this, SLOT( showStatusBarContextMenu( QPoint )));

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

    connect( m_showPositionAction, SIGNAL( triggered( bool ) ),
             this, SLOT( showPositionLabel( bool ) ) );
    connect( m_showAltitudeAction, SIGNAL( triggered( bool ) ),
             this, SLOT( showAltitudeLabel( bool ) ) );
    connect( m_showTileZoomLevelAction, SIGNAL( triggered( bool ) ),
             this, SLOT( showTileZoomLevelLabel( bool ) ) );
    connect( m_showDateTimeAction, SIGNAL( triggered( bool ) ),
             this, SLOT( showDateTimeLabel( bool ) ) );
    connect( m_showDownloadProgressAction, SIGNAL( triggered( bool ) ),
             this, SLOT( showDownloadProgressBar( bool ) ) );
}

void MarblePart::showNewStuffDialog()
{
    QString  newStuffConfig = KStandardDirs::locate ( "data",
                                                      "marble/marble.knsrc" );
    kDebug() << "KNS config file:" << newStuffConfig;

    QPointer<KNS3::DownloadDialog> dialog(new KNS3::DownloadDialog(newStuffConfig, m_controlView));
    dialog->exec();
    delete dialog;
}

void MarblePart::showUploadNewStuffDialog()
{
    QString  newStuffConfig = KStandardDirs::locate ( "data", "marble/marble.knsrc" );
    kDebug() << "KNS config file:" << newStuffConfig;

    QPointer<KNS3::UploadDialog> dialog( new KNS3::UploadDialog( newStuffConfig, m_controlView ) );
    kDebug() << "Creating the archive";
    dialog->setUploadFile( KUrl( MapWizard::createArchive( m_controlView, m_controlView->marbleWidget()->mapThemeId() ) ) );
    dialog->exec();
    MapWizard::deleteArchive( m_controlView->marbleWidget()->mapThemeId() );
    delete dialog;
}

// connect to expensive slots, only needed when the non modal dialog is show
void MarblePart::connectDownloadRegionDialog()
{
    connect( m_controlView->marbleWidget(), SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox )),
             m_downloadRegionDialog, SLOT( setVisibleLatLonAltBox( GeoDataLatLonAltBox )));
    connect( m_controlView->marbleWidget(), SIGNAL( themeChanged( QString )),
             m_downloadRegionDialog, SLOT( updateTextureLayer() ));
}

// disconnect from expensive slots, not needed when dialog is hidden
void MarblePart::disconnectDownloadRegionDialog()
{
    disconnect( m_controlView->marbleWidget(), SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox )),
                m_downloadRegionDialog, SLOT( setVisibleLatLonAltBox( GeoDataLatLonAltBox )));
    disconnect( m_controlView->marbleWidget(), SIGNAL( themeChanged( QString )),
                m_downloadRegionDialog, SLOT( updateTextureLayer() ));
}

void MarblePart::showDownloadRegionDialog()
{
    MarbleWidget * const marbleWidget = m_controlView->marbleWidget();
    if ( !m_downloadRegionDialog ) {
        m_downloadRegionDialog = new DownloadRegionDialog( marbleWidget, widget() );
        // it might be tempting to move the connects to DownloadRegionDialog's "accepted" and
        // "applied" signals, be aware that the "hidden" signal might be come before the "accepted"
        // signal, leading to a too early disconnect.
        connect( m_downloadRegionDialog, SIGNAL( accepted() ), SLOT( downloadRegion() ));
        connect( m_downloadRegionDialog, SIGNAL( applied() ), SLOT( downloadRegion() ));
        connect( m_downloadRegionDialog, SIGNAL( shown() ), SLOT( connectDownloadRegionDialog() ));
        connect( m_downloadRegionDialog, SIGNAL( hidden() ),
                 SLOT( disconnectDownloadRegionDialog() ));
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
    KStatusBar * const statusBar = m_statusBarExtension->statusBar();
    Q_ASSERT( statusBar );

    KMenu statusBarContextMenu( m_controlView->marbleWidget() );
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

    // It's experimental -- so we remove it for now.
    // FIXME: Delete the following  line once OpenGL support is officially supported.
    ui_viewSettings.kcfg_graphicsSystem->removeItem( OpenGLGraphics );

    QString nativeString ( i18n("Native") );

    #ifdef Q_WS_X11
    nativeString = i18n( "Native (X11)" );
    #endif
    #ifdef Q_WS_MAC
    nativeString = i18n( "Native (Mac OS X Core Graphics)" );
    #endif

    ui_viewSettings.kcfg_graphicsSystem->setItemText( NativeGraphics, nativeString );

    // navigation page
    Ui_MarbleNavigationSettingsWidget  ui_navigationSettings;
    QWidget                           *w_navigationSettings = new QWidget( 0 );

    w_navigationSettings->setObjectName( "navigation_page" );
    ui_navigationSettings.setupUi( w_navigationSettings );
    m_configDialog->addPage( w_navigationSettings, i18n( "Navigation" ),
                             "transform-move" );

    // cache page
    MarbleCacheSettingsWidget *w_cacheSettings = new MarbleCacheSettingsWidget();
    w_cacheSettings->setObjectName( "cache_page" );
    m_configDialog->addPage( w_cacheSettings, i18n( "Cache & Proxy" ),
                             "preferences-web-browser-cache" );
    connect( w_cacheSettings,               SIGNAL( clearVolatileCache() ),
             m_controlView->marbleWidget(), SLOT( clearVolatileTileCache() ) );
    connect( w_cacheSettings,                        SIGNAL( clearPersistentCache() ),
             m_controlView->marbleModel(), SLOT( clearPersistentTileCache() ) );

    // time page
    Ui_MarbleTimeSettingsWidget ui_timeSettings;
    QWidget *w_timeSettings = new QWidget( 0 );

    w_timeSettings->setObjectName( "time_page" );
    ui_timeSettings.setupUi( w_timeSettings );
    m_configDialog->addPage( w_timeSettings, i18n( "Date & Time" ), "clock" );

    // routing page
    RoutingProfilesWidget *w_routingSettings = new RoutingProfilesWidget( m_controlView->marbleModel() );
    w_routingSettings->setObjectName( "routing_page" );
    m_configDialog->addPage( w_routingSettings, tr( "Routing" ) );

    // plugin page
    m_pluginModel = new RenderPluginModel( this );
    m_pluginModel->setRenderPlugins( m_controlView->marbleWidget()->renderPlugins() );

    MarblePluginSettingsWidget *w_pluginSettings = new MarblePluginSettingsWidget();
    w_pluginSettings->setModel( m_pluginModel );
    w_pluginSettings->setObjectName( "plugin_page" );
    m_configDialog->addPage( w_pluginSettings, i18n( "Plugins" ),
                             "preferences-plugin" );
    // Setting the icons of the pluginSettings page.
    w_pluginSettings->setConfigIcon( KIcon( "configure" ) );
    w_pluginSettings->setAboutIcon( KIcon( "help-about" ) );

    connect( w_pluginSettings, SIGNAL( pluginListViewClicked() ),
                               SLOT( enableApplyButton() ) );
    connect( m_configDialog,   SIGNAL( settingsChanged( const QString &) ),
                               SLOT( updateSettings() ) );
    connect( m_configDialog,   SIGNAL( applyClicked() ),
                               SLOT( applyPluginState() ) );
    connect( m_configDialog,   SIGNAL( okClicked() ),
                               SLOT( applyPluginState() ) );
    connect( m_configDialog,   SIGNAL( cancelClicked() ),
             m_pluginModel,    SLOT( retrievePluginState() ) );

    m_configDialog->show();
}

void MarblePart::enableApplyButton()
{
    m_configDialog->enableButtonApply( true );
}

void MarblePart::applyPluginState()
{
    m_pluginModel->applyPluginState();

    QList<RoutingProfile>  profiles = m_controlView->marbleWidget()
                        ->model()->routingManager()->profilesModel()->profiles();
    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig( KGlobal::mainComponent() );
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
                Q_ASSERT( settingKey != "Enabled" );
                pluginGroup.writeEntry( settingKey, profile.pluginSettings()[ key ][ settingKey ] );
            }
        }
    }
}

void MarblePart::updateSettings()
{
    kDebug() << "Updating Settings ...";

    // FIXME: Font doesn't get updated instantly.
    m_controlView->marbleWidget()->setDefaultFont( MarbleSettings::mapFont() );

    m_controlView->marbleWidget()->
        setMapQualityForViewContext( (MapQuality) MarbleSettings::stillQuality(),
                                     Still );
    m_controlView->marbleWidget()->
        setMapQualityForViewContext( (MapQuality) MarbleSettings::animationQuality(),
                                     Animation );

    GraphicsSystem graphicsSystem = (GraphicsSystem) MarbleSettings::graphicsSystem();

    m_controlView->marbleWidget()->
        setDefaultAngleUnit( (AngleUnit) MarbleSettings::angleUnit() );
    MarbleGlobal::getInstance()->locale()->
        setMeasurementSystem( (QLocale::MeasurementSystem) MarbleSettings::distanceUnit() );

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
    if ( MarbleSettings::proxyUrl().isEmpty() || MarbleSettings::proxyUrl() == "http://" ) {
        proxy.setType( QNetworkProxy::NoProxy );
    } else {
        if ( MarbleSettings::proxyType() == Marble::Socks5Proxy ) {
            proxy.setType( QNetworkProxy::Socks5Proxy );
        }
        else if ( MarbleSettings::proxyType() == Marble::HttpProxy ) {
            proxy.setType( QNetworkProxy::HttpProxy );
        }
        else {
            kDebug() << "Unknown proxy type! Using Http Proxy instead.";
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

    // Show message box
    if (    m_initialGraphicsSystem != graphicsSystem
         && m_previousGraphicsSystem != graphicsSystem ) {
        KMessageBox::information (m_controlView->marbleWidget(),
                                i18n("You have decided to run Marble with a different graphics system.\n"
                                   "For this change to become effective, Marble has to be restarted.\n"
                                   "Please close the application and start Marble again."),
                                i18n("Graphics System Change") );
    }
    m_previousGraphicsSystem = graphicsSystem;

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
}

void MarblePart::writePluginSettings()
{
    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig( KGlobal::mainComponent() );

    foreach( RenderPlugin *plugin, m_controlView->marbleWidget()->renderPlugins() ) {
        KConfigGroup group = sharedConfig->group( QString( "plugin_" ) + plugin->nameId() );

        QHash<QString,QVariant> hash = plugin->settings();

        QHash<QString,QVariant>::iterator it = hash.begin();
        while( it != hash.end() ) {
            group.writeEntry( it.key(), it.value() );
            ++it;
        }
        group.sync();
    }
}

void MarblePart::readPluginSettings()
{
    KSharedConfig::Ptr sharedConfig = KSharedConfig::openConfig( KGlobal::mainComponent() );

    foreach( RenderPlugin *plugin, m_controlView->marbleWidget()->renderPlugins() ) {
        KConfigGroup group = sharedConfig->group( QString( "plugin_" ) + plugin->nameId() );

        QHash<QString,QVariant> hash = plugin->settings();

        foreach ( const QString& key, group.keyList() ) {
            hash.insert( key, group.readEntry( key ) );
        }

        plugin->setSettings( hash );
    }
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

void MarblePart::downloadJobAdded()
{
    m_downloadProgressBar->setUpdatesEnabled( false );
    if ( m_downloadProgressBar->value() < 0 ) {
        m_downloadProgressBar->setMaximum( 1 );
        m_downloadProgressBar->setValue( 0 );
        m_downloadProgressBar->setVisible( MarbleSettings::showDownloadProgressBar() );
    } else {
        m_downloadProgressBar->setMaximum( m_downloadProgressBar->maximum() + 1 );
    }

//     kDebug() << "downloadProgressJobAdded: value/maximum: "
//              << m_downloadProgressBar->value() << '/' << m_downloadProgressBar->maximum();

    m_downloadProgressBar->setUpdatesEnabled( true );
}

void MarblePart::downloadJobRemoved()
{
    m_downloadProgressBar->setUpdatesEnabled( false );
    m_downloadProgressBar->setValue( m_downloadProgressBar->value() + 1 );
    if ( m_downloadProgressBar->value() == m_downloadProgressBar->maximum() ) {
        m_downloadProgressBar->reset();
        m_downloadProgressBar->setVisible( false );
    }

//     kDebug() << "downloadProgressJobCompleted: value/maximum: "
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
    QPointer<QPrintDialog> printDialog = KdePrint::createPrintDialog(&printer, widget());
    m_controlView->printMapScreenShot( printDialog );
    delete printDialog;
#endif
}

void MarblePart::updateMapEditButtonVisibility( const QString &mapTheme )
{
    Q_ASSERT( m_externalMapEditorAction );
    m_externalMapEditorAction->setVisible( mapTheme == "earth/openstreetmap/openstreetmap.dgml" );
}

void MarblePart::setupToolBar( KToolBar *toolBar )
{
    m_searchField = new SearchInputWidget( toolBar );
    m_searchField->setCompletionModel( m_controlView->marbleModel()->placemarkModel() );
    m_searchField->setMaximumWidth( 400 );
    connect( m_searchField, SIGNAL( search( QString, SearchMode ) ), m_controlView, SLOT( search( QString, SearchMode ) ) );
    connect( m_searchField, SIGNAL( centerOn( const GeoDataCoordinates & ) ),
             m_controlView->marbleWidget(), SLOT( centerOn( const GeoDataCoordinates &) ) );
    connect( m_controlView, SIGNAL( searchFinished() ), m_searchField, SLOT( disableSearchAnimation() ) );

    QKeySequence searchShortcut( Qt::CTRL + Qt::Key_F );
    m_searchField->setToolTip( QString( "Search for cities, addresses, points of interest and more (%1)" ).arg( searchShortcut.toString() ) );
    new QShortcut( searchShortcut, m_searchField, SLOT( setFocus() ) );
    toolBar->addWidget( m_searchField );
}

}

#include "marble_part.moc"
