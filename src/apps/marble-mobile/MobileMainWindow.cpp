//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2010 Torsten Rahn        <tackat@kde.org>
// Copyright 2007      Inge Wallin         <ingwa@kde.org>
// Copyright 2010-2013 Bernhard Beschow    <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
// Copyright 2012      Mohammed Nafees     <nafees.technocool@gmail.com>
//

#include "MobileMainWindow.h"

#include "StackableWindow.h"

// Marble
#include "BookmarkManager.h"
#include "BookmarkManagerDialog.h"
#include "CurrentLocationWidget.h"
#include "DownloadRegionDialog.h"
#include "GoToDialog.h"
#include "MapViewWidget.h"
#include "MarbleAboutDialog.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleLegendBrowser.h"
#include "MarbleLocale.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "MarbleWidgetInputHandler.h"
#include "Planet.h"
#include "PluginManager.h"
#include "PositionTracking.h"
#include "PositionProviderPlugin.h"
#include "ViewportParams.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingProfilesModel.h"
#include "routing/RoutingWidget.h"
#include "routing/RouteRequest.h"

#include <QList>
#include <QSettings>
#include <QCloseEvent>
#include <QVariantMap>
#include <QVector>

#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMenuBar>
#include <QScrollArea>
#include <QSplitter>

// For zoom buttons on Maemo
#ifdef Q_WS_MAEMO_5
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif // Q_WS_MAEMO_5

namespace {

const QString defaultMapThemeId = "earth/openstreetmap/openstreetmap.dgml";
        // "earth/srtm/srtm.dgml"
        // "earth/bluemarble/bluemarble.dgml"

}

using namespace Marble;
/* TRANSLATOR Marble::MainWindow */

MainWindow::MainWindow( const QString &marbleDataPath, const QVariantMap &cmdLineSettings, QWidget *parent ) :
        QMainWindow( parent ),
        m_marbleWidget( new MarbleWidget( this ) ),
        m_legendBrowser( new MarbleLegendBrowser( this ) ),
        m_downloadRegionDialog( 0 ),
        m_mapViewDialog( 0 ),
        m_routingWindow( 0 ),
        m_trackingWindow( 0 ),
        m_gotoDialog( 0 ),
        m_routingWidget( 0 ),
        m_workOfflineAct( 0 ),
        m_kineticScrollingAction( 0 ),
        m_showLegendAct( 0 )
{
#ifdef Q_WS_MAEMO_5
    setAttribute( Qt::WA_Maemo5StackedWindow );
#endif // Q_WS_MAEMO_5
    setUpdatesEnabled( false );

    m_legendBrowser->setMarbleModel( m_marbleWidget->model() );
    connect( m_legendBrowser, SIGNAL(toggledShowProperty(QString,bool)),
             m_marbleWidget, SLOT(setPropertyValue(QString,bool)), Qt::QueuedConnection );

    // prevent triggering of network requests under Maemo, presumably due to qrc: URLs
    m_networkAccessManager.setNetworkAccessible( QNetworkAccessManager::NotAccessible );
    m_legendBrowser->page()->setNetworkAccessManager( &m_networkAccessManager );

    QString selectedPath = marbleDataPath.isEmpty() ? readMarbleDataPath() : marbleDataPath;
    if ( !selectedPath.isEmpty() )
        MarbleDirs::setMarbleDataPath( selectedPath );

    QSplitter *splitter = new QSplitter( this );
    splitter->setOrientation( Qt::Horizontal );
    splitter->addWidget( m_legendBrowser );
    splitter->setStretchFactor( 0, 0 );
    splitter->addWidget( m_marbleWidget );
    splitter->setStretchFactor( 1, 1 );
    splitter->setSizes( QList<int>() << 180 << width() - 180 );

    setWindowTitle( tr( "Marble - Virtual Globe" ) );
    setWindowIcon( QIcon( ":/icons/marble.png" ) );
    setCentralWidget( splitter );

    m_workOfflineAct = menuBar()->addAction( tr( "Work Off&line" ) );
    m_workOfflineAct->setIcon( QIcon( ":/icons/user-offline.png" ) );
    m_workOfflineAct->setCheckable( true );
    m_workOfflineAct->setChecked( m_marbleWidget->model()->workOffline() );
    connect( m_workOfflineAct, SIGNAL(triggered(bool)), this, SLOT(setWorkOffline(bool)) );

    m_kineticScrollingAction = menuBar()->addAction( QApplication::translate("MarbleNavigationSettingsWidget", "&Inertial Globe Rotation", 0, QApplication::UnicodeUTF8) );
    m_kineticScrollingAction->setCheckable( true );
    connect( m_kineticScrollingAction, SIGNAL(triggered(bool)), this, SLOT(setKineticScrollingEnabled(bool)) );

    /** @todo: Full screen cannot be left on Maemo currently (shortcuts not working) */
    //menuBar()->addAction( m_fullScreenAct );

    QAction *const downloadRegionAction = menuBar()->addAction( tr( "Download &Region..." ) );
    downloadRegionAction->setStatusTip( tr( "Download a map region in different zoom levels for offline usage" ) );
    connect( downloadRegionAction, SIGNAL(triggered()), SLOT(showDownloadRegionDialog()) );

    QAction *const showMapViewDialogAction = menuBar()->addAction( tr( "Map View" ) );
    connect( showMapViewDialogAction, SIGNAL(triggered(bool)), this, SLOT(showMapViewDialog()) );

    m_showLegendAct = menuBar()->addAction( tr( "Legend" ) );
    m_showLegendAct->setCheckable( true );
    connect( m_showLegendAct, SIGNAL(toggled(bool)), this, SLOT(setLegendShown(bool)) );

    QAction *const m_toggleRoutingTabAction = menuBar()->addAction( tr( "Routing" ) );
    connect( m_toggleRoutingTabAction, SIGNAL(triggered(bool)), this, SLOT(showRoutingDialog()) );

    QAction *const m_showTrackingDialogAction = menuBar()->addAction( tr( "Tracking" ) );
    connect( m_showTrackingDialogAction, SIGNAL(triggered()), this, SLOT(showTrackingDialog()) );

    QAction *goToAction = menuBar()->addAction( tr( "&Go To...") );
    connect( goToAction, SIGNAL(triggered()), this, SLOT(showGoToDialog()) );

    QAction *const manageBookmarksAct = menuBar()->addAction( tr( "&Manage Bookmarks" ) );
    manageBookmarksAct->setIcon( QIcon( ":/icons/bookmarks-organize.png" ) );
    manageBookmarksAct->setStatusTip( tr( "Manage Bookmarks" ) );
    connect( manageBookmarksAct, SIGNAL(triggered()), this, SLOT(showBookmarkManagerDialog()) );

    QAction *const aboutMarbleAct = menuBar()->addAction( tr( "&About Marble Virtual Globe" ) );
    aboutMarbleAct->setIcon( QIcon( ":/icons/marble.png" ) );
    aboutMarbleAct->setStatusTip( tr( "Show the application's About Box" ) );
    connect( aboutMarbleAct, SIGNAL(triggered()), this, SLOT(showAboutMarbleDialog()) );

    // setup zoom buttons
#ifdef Q_WS_MAEMO_5
    if ( winId() ) {
        Atom atom = XInternAtom( QX11Info::display(), "_HILDON_ZOOM_KEY_ATOM", False );
        if ( atom ) {
            unsigned long val = 1;
            XChangeProperty ( QX11Info::display(), winId(), atom, XA_INTEGER, 32,
                             PropModeReplace, reinterpret_cast<unsigned char *>( &val ), 1 );

            QAction* zoomIn = new QAction( tr( "Zoom &In" ), this );
            zoomIn->setShortcut( Qt::Key_F7 );
            connect( zoomIn, SIGNAL(triggered()), m_marbleWidget, SLOT(zoomIn()) );
            addAction( zoomIn );

            QAction* zoomOut = new QAction( tr( "Zoom &Out" ), this );
            zoomOut->setShortcut( Qt::Key_F8 );
            connect( zoomOut, SIGNAL(triggered()), m_marbleWidget, SLOT(zoomOut()) );
            addAction( zoomOut );
        }
    }
#endif // Q_WS_MAEMO_5

    connect( &m_mapThemeManager, SIGNAL(themesChanged()), this, SLOT(fallBackToDefaultTheme()) );

    setUpdatesEnabled( true );

    QMetaObject::invokeMethod( this,
                               "initObject", Qt::QueuedConnection,
                               Q_ARG( QVariantMap, cmdLineSettings ) );
}

MarbleWidget *MainWindow::marbleWidget()
{
    return m_marbleWidget;
}

void MainWindow::addGeoDataFile( const QString &path )
{
    // delay file loading to initObject(), such that restoring view from previous session in readSettings()
    // doesn't interfere with focusing on these files
    m_commandlineFilePaths << path;
}

void MainWindow::initObject( const QVariantMap &cmdLineSettings )
{
    QCoreApplication::processEvents ();
    readSettings( cmdLineSettings );

    foreach ( const QString &path, m_commandlineFilePaths ) {
        m_marbleWidget->model()->addGeoDataFile( path );
    }
}

void MainWindow::showBookmarkManagerDialog()
{
    MarbleModel * const model = m_marbleWidget->model();
    QPointer<BookmarkManagerDialog> dialog = new BookmarkManagerDialog( model, this );
#ifdef Q_WS_MAEMO_5
    dialog->setButtonBoxVisible( false );
    dialog->setAttribute( Qt::WA_Maemo5StackedWindow );
    dialog->setWindowFlags( Qt::Window );
#endif // Q_WS_MAEMO_5
    dialog->exec();
    delete dialog;
}

#ifdef Q_WS_MAEMO_5
void MainWindow::setOrientation( Orientation orientation )
{
    switch ( orientation ) {
    case OrientationAutorotate:
       setAttribute( Qt::WA_Maemo5AutoOrientation );
       break;
    case OrientationLandscape:
       setAttribute( Qt::WA_Maemo5LandscapeOrientation );
       break;
    case OrientationPortrait:
       setAttribute( Qt::WA_Maemo5PortraitOrientation );
       break;
    }
}

MainWindow::Orientation MainWindow::orientation() const
{
    if ( testAttribute( Qt::WA_Maemo5LandscapeOrientation ) )
        return OrientationLandscape;

    if ( testAttribute( Qt::WA_Maemo5PortraitOrientation ) )
        return OrientationPortrait;

    return OrientationAutorotate;
}
#endif // Q_WS_MAEMO_5

void MainWindow::setWorkOffline( bool offline )
{
    m_marbleWidget->model()->setWorkOffline( offline );
    if ( !offline ) {
        m_marbleWidget->clearVolatileTileCache();
    }

    m_workOfflineAct->setChecked( offline );
}

void MainWindow::setKineticScrollingEnabled( bool enabled )
{
    m_marbleWidget->inputHandler()->setInertialEarthRotationEnabled( enabled );
    m_kineticScrollingAction->setChecked( enabled ); // Sync state with the GUI
}

void MainWindow::setLegendShown( bool show )
{
    m_legendBrowser->setVisible( show );
    m_showLegendAct->setChecked( show );
}

void MainWindow::showAboutMarbleDialog()
{
    MarbleAboutDialog dlg( this );
    dlg.setApplicationTitle( tr( "Marble Virtual Globe %1" ).arg( "MARBLE_MOBILE_VERSION_STRING" ) );
    dlg.exec();
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    writeSettings();
    event->accept();
}

QString MainWindow::readMarbleDataPath()
{
    QSettings settings;

    settings.beginGroup( "MarbleWidget" );
        QString marbleDataPath;
        marbleDataPath = settings.value( "marbleDataPath", "" ).toString(),
    settings.endGroup();

    return marbleDataPath;
}

void MainWindow::readSettings(const QVariantMap& overrideSettings)
{
    // Load bookmark file. If it does not exist, a default one will be used.
    m_marbleWidget->model()->bookmarkManager()->loadFile( "bookmarks/bookmarks.kml" );

    QSettings settings;

    settings.beginGroup( "MainWindow" );
        resize( settings.value( "size", QSize( 640, 480 ) ).toSize() );
        move( settings.value( "pos", QPoint( 200, 200 ) ).toPoint() );
        if ( settings.value( "fullScreen", false ).toBool() ) {
           showFullScreen();
        }
#ifdef Q_WS_MAEMO_5
        const Orientation orientation = (Orientation)settings.value( "orientation", (int)OrientationLandscape ).toInt();
        setOrientation( orientation );
#endif // Q_WS_MAEMO_5
        show();
        setWorkOffline( settings.value( "workOffline", false ).toBool() );
        setLegendShown( settings.value( "showLegend", false ).toBool() );
        m_marbleWidget->setShowAtmosphere( settings.value( "showAtmosphere", true ).toBool() );
        m_lastFileOpenPath = settings.value( "lastFileOpenDir", QDir::homePath() ).toString();
        m_marbleWidget->model()->bookmarkManager()->setShowBookmarks( settings.value( "showBookmarks", true ).toBool() );
        restoreState( settings.value( "windowState" ).toByteArray() );
    settings.endGroup();

    setUpdatesEnabled( false );

    settings.beginGroup( "MarbleWidget" );
        QString mapThemeId;
        const QVariantMap::ConstIterator mapThemeIdIt = overrideSettings.find( QLatin1String( "mapTheme" ) );
        if ( mapThemeIdIt != overrideSettings.constEnd() ) {
           mapThemeId = mapThemeIdIt.value().toString();
        } else {
           mapThemeId = settings.value( "mapTheme", defaultMapThemeId ).toString();
        }
        mDebug() << Q_FUNC_INFO << "mapThemeId:" << mapThemeId;
        m_marbleWidget->setMapThemeId( mapThemeId );
        m_marbleWidget->setProjection( (Projection)settings.value( "projection", Mercator ).toInt() );

        // Set home position
        m_marbleWidget->model()->setHome(
           settings.value( "homeLongitude", 9.4 ).toDouble(),
           settings.value( "homeLatitude", 54.8 ).toDouble(),
           settings.value( "homeZoom", 1050 ).toInt()
        );

        // Center on/Distance
        const QVariantMap::ConstIterator distanceIt = overrideSettings.find( QLatin1String( "distance" ) );
        const bool isDistanceOverwritten = (distanceIt != overrideSettings.constEnd());

        const QVariantMap::ConstIterator lonLatIt = overrideSettings.find( QLatin1String( "lonlat" ) );
        if ( lonLatIt != overrideSettings.constEnd() ) {
           const QVariantList lonLat = lonLatIt.value().toList();
           m_marbleWidget->centerOn( lonLat.at( 0 ).toDouble(), lonLat.at( 1 ).toDouble() );
        } else {
           m_marbleWidget->centerOn(
               settings.value( "quitLongitude", 0.0 ).toDouble(),
               settings.value( "quitLatitude", 0.0 ).toDouble() );
        }
        if ( isDistanceOverwritten ) {
            m_marbleWidget->setDistance( distanceIt.value().toDouble() );
        } else {
            // set default radius to 1350 (Atlas theme's "sharp" radius)
            m_marbleWidget->setRadius( settings.value( "quitRadius", 1350 ).toInt() );
        }
    settings.endGroup();

    setUpdatesEnabled( true );

    // Load previous route settings
    settings.beginGroup( "Routing" );
    {
        RoutingManager *const routingManager = m_marbleWidget->model()->routingManager();
        routingManager->readSettings();
        bool const startupWarning = settings.value( "showGuidanceModeStartupWarning", QVariant( true ) ).toBool();
        routingManager->setShowGuidanceModeStartupWarning( startupWarning );
        routingManager->setLastOpenPath( settings.value( "lastRouteOpenPath", QDir::homePath() ).toString() );
        routingManager->setLastSavePath( settings.value( "lastRouteSavePath", QDir::homePath() ).toString() );

        QColor tempColor;
        tempColor = QColor( settings.value( "routeColorStandard", Oxygen::skyBlue4.name() ).toString() );
        tempColor.setAlpha( settings.value( "routeAlphaStandard", 200 ).toInt() );
        routingManager->setRouteColorStandard( tempColor );

        tempColor = QColor( settings.value( "routeColorHighlighted", Oxygen::skyBlue1.name() ).toString() );
        tempColor.setAlpha( settings.value( "routeAlphaHighlighted", 200 ).toInt() );
        routingManager->setRouteColorHighlighted( tempColor );

        tempColor = QColor( settings.value( "routeColorAlternative", Oxygen::aluminumGray4.name() ).toString() );
        tempColor.setAlpha( settings.value( "routeAlphaAlternative", 200 ).toInt() );
        routingManager->setRouteColorAlternative( tempColor );
    }
    settings.endGroup();

    settings.beginGroup( "Routing Profile" );
        if ( settings.contains( "Num" ) ) {
            QList<RoutingProfile> profiles;
            int numProfiles = settings.value( "Num", 0 ).toInt();
            for ( int i = 0; i < numProfiles; ++i ) {
                settings.beginGroup( QString( "Profile %0" ).arg(i) );
                QString name = settings.value( "Name", tr( "Unnamed" ) ).toString();
                RoutingProfile profile( name );
                foreach ( const QString& pluginName, settings.childGroups() ) {
                    settings.beginGroup( pluginName );
                    profile.pluginSettings().insert( pluginName, QHash<QString, QVariant>() );
                    foreach ( const QString& key, settings.childKeys() ) {
                        if ( key != "Enabled" ) {
                            profile.pluginSettings()[ pluginName ].insert( key, settings.value( key ) );
                        }
                    }
                    settings.endGroup();
                }
                profiles << profile;
                settings.endGroup();
            }
            m_marbleWidget->model()->routingManager()->profilesModel()->setProfiles( profiles );
        } else {
            m_marbleWidget->model()->routingManager()->profilesModel()->loadDefaultProfiles();
        }
        int const profileIndex = settings.value( "currentIndex", 0 ).toInt();
        if ( profileIndex >= 0 && profileIndex < m_marbleWidget->model()->routingManager()->profilesModel()->rowCount() ) {
            RoutingProfile profile = m_marbleWidget->model()->routingManager()->profilesModel()->profiles().at( profileIndex );
            m_marbleWidget->model()->routingManager()->routeRequest()->setRoutingProfile( profile );
        }
    settings.endGroup();

    settings.beginGroup( "Plugins");
        PositionTracking* tracking = m_marbleWidget->model()->positionTracking();
        tracking->readSettings();
        QString positionProvider = settings.value( "activePositionTrackingPlugin", QString() ).toString();
        if ( !positionProvider.isEmpty() ) {
            const PluginManager* pluginManager = m_marbleWidget->model()->pluginManager();
            foreach( const PositionProviderPlugin* plugin, pluginManager->positionProviderPlugins() ) {
                if ( plugin->nameId() == positionProvider ) {
                    PositionProviderPlugin* instance = plugin->newInstance();
                    instance->setMarbleModel( m_marbleWidget->model() );
                    tracking->setPositionProviderPlugin( instance );
                    break;
                }
            }
        }
    settings.endGroup();

    settings.beginGroup( "Tracking" );
    if ( settings.contains( "autoCenter" ) || settings.contains( "recenterMode" ) ) {
        initializeTrackingWidget();
        CurrentLocationWidget* trackingWidget = qobject_cast<CurrentLocationWidget*>( m_trackingWindow->centralWidget() );
        Q_ASSERT( trackingWidget );
        trackingWidget->setRecenterMode( settings.value( "recenterMode", 0 ).toInt() );
        trackingWidget->setAutoZoom( settings.value( "autoZoom", false ).toBool() );
        trackingWidget->setTrackVisible( settings.value( "trackVisible", true ).toBool() );
        trackingWidget->setLastOpenPath( settings.value( "lastTrackOpenPath", QDir::homePath() ).toString() );
        trackingWidget->setLastSavePath( settings.value( "lastTrackSavePath", QDir::homePath() ).toString() );
    }
    settings.endGroup();

    settings.beginGroup( "View" );
        m_marbleWidget->setDefaultFont( settings.value( "mapFont", QApplication::font() ).value<QFont>() );
        m_marbleWidget->setMapQualityForViewContext( static_cast<Marble::MapQuality>( settings.value( "stillQuality", Marble::HighQuality ).toInt() ), Marble::Still );
        m_marbleWidget->setMapQualityForViewContext( static_cast<Marble::MapQuality>( settings.value( "animationQuality", Marble::LowQuality ).toInt() ), Marble::Animation );
        m_marbleWidget->setDefaultAngleUnit( static_cast<Marble::AngleUnit>( settings.value( "angleUnit", Marble::DMSDegree ).toInt() ) );
    settings.endGroup();

    settings.beginGroup( "Navigation" );
        m_marbleWidget->setAnimationsEnabled( settings.value( "animateTargetVoyage", true ).toBool() );
        setKineticScrollingEnabled( settings.value( "inertialEarthRotation", true ).toBool() );
    settings.endGroup();

    settings.beginGroup( "Cache" );
        m_marbleWidget->model()->setPersistentTileCacheLimit( settings.value( "persistentTileCacheLimit", 0 ).toInt() * 1024 ); // default to unlimited
        m_marbleWidget->setVolatileTileCacheLimit( settings.value( "volatileTileCacheLimit", 6 ).toInt() * 1024 );
    settings.endGroup();

    m_marbleWidget->readPluginSettings( settings );
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup( "MainWindow" );
        settings.setValue( "size", size() );
        settings.setValue( "pos", pos() );
        settings.setValue( "fullScreen", isFullScreen() );
#ifdef Q_WS_MAEMO_5
        settings.setValue( "orientation", (int)orientation() );
#endif // Q_WS_MAEMO_5
        settings.setValue( "workOffline", m_marbleWidget->model()->workOffline() );
        settings.setValue( "showLegend", m_legendBrowser->isVisible() );
        settings.setValue( "showAtmosphere", m_marbleWidget->showAtmosphere() );
        settings.setValue( "lastFileOpenDir", m_lastFileOpenPath );
        settings.setValue( "windowState", saveState() );
    settings.endGroup();

    settings.beginGroup( "MarbleWidget" );
        // Get the 'home' values from the widget and store them in the settings.
        qreal homeLon = 0;
        qreal homeLat = 0;
        int homeZoom = 0;
        m_marbleWidget->model()->home( homeLon, homeLat, homeZoom );

        settings.setValue( "homeLongitude", homeLon );
        settings.setValue( "homeLatitude",  homeLat );
        settings.setValue( "homeZoom",      homeZoom );

        settings.setValue( "mapTheme",   m_marbleWidget->mapThemeId() );
        settings.setValue( "projection", m_marbleWidget->projection() );

        settings.setValue( "quitLongitude", m_marbleWidget->centerLongitude() );
        settings.setValue( "quitLatitude", m_marbleWidget->centerLatitude() );
        settings.setValue( "quitRadius", m_marbleWidget->radius() );
    settings.endGroup();

    settings.beginGroup( "Routing Profile" );
        QList<RoutingProfile> profiles = m_marbleWidget->model()->routingManager()->profilesModel()->profiles();
        settings.setValue( "Num", profiles.count() );
        for ( int i = 0; i < profiles.count(); ++i ) {
            settings.beginGroup( QString( "Profile %0" ).arg(i) );
            const RoutingProfile& profile = profiles.at( i );
            settings.setValue( "Name", profile.name() );
            foreach ( const QString& pluginName, settings.childGroups() ) {
                settings.beginGroup( pluginName );
                settings.remove( "" ); //remove all keys
                settings.endGroup();
            }
            foreach ( const QString &key, profile.pluginSettings().keys() ) {
                settings.beginGroup( key );
                settings.setValue( "Enabled", true );
                foreach ( const QString& settingKey, profile.pluginSettings()[ key ].keys() ) {
                    Q_ASSERT( settingKey != "Enabled" );
                    settings.setValue( settingKey, profile.pluginSettings()[ key ][ settingKey ] );
                }
                settings.endGroup();
            }
            settings.endGroup();
        }
        RoutingProfile const profile = m_marbleWidget->model()->routingManager()->routeRequest()->routingProfile();
        settings.setValue( "currentIndex", profiles.indexOf( profile ) );
    settings.endGroup();

    settings.beginGroup( "Plugins");
        QString positionProvider;
        PositionTracking* tracking = m_marbleWidget->model()->positionTracking();
        tracking->writeSettings();
        if ( tracking && tracking->positionProviderPlugin() ) {
            positionProvider = tracking->positionProviderPlugin()->nameId();
        }
        settings.setValue( "activePositionTrackingPlugin", positionProvider );
    settings.endGroup();

    settings.beginGroup( "Tracking" );
        Q_ASSERT( m_trackingWindow );
        CurrentLocationWidget* trackingWidget = static_cast<CurrentLocationWidget *>( m_trackingWindow->centralWidget() );
        if ( trackingWidget ) {
            // Can be null due to lazy initialization
            settings.setValue( "recenterMode", trackingWidget->recenterMode() );
            settings.setValue( "autoZoom", trackingWidget->autoZoom() );
            settings.setValue( "trackVisible", trackingWidget->trackVisible() );
            settings.setValue( "lastTrackOpenPath", trackingWidget->lastOpenPath() );
            settings.setValue( "lastTrackSavePath", trackingWidget->lastSavePath() );
        }
    settings.endGroup();

    settings.beginGroup( "Navigation" );
        settings.setValue( "inertialEarthRotation", m_marbleWidget->inputHandler()->inertialEarthRotationEnabled() );
    settings.endGroup();

    // Store current route settings
    settings.beginGroup( "Routing" );
    {
        RoutingManager *const routingManager = m_marbleWidget->model()->routingManager();
        routingManager->writeSettings();
        settings.setValue( "showGuidanceModeStartupWarning", routingManager->showGuidanceModeStartupWarning() );
        settings.setValue( "lastRouteOpenPath", routingManager->lastOpenPath() );
        settings.setValue( "lastRouteSavePath", routingManager->lastSavePath() );
        settings.setValue( "routeColorStandard", routingManager->routeColorStandard().name() );
        settings.setValue( "routeAlphaStandard", routingManager->routeColorStandard().alpha() );
        settings.setValue( "routeColorHighlighted", routingManager->routeColorHighlighted().name() );
        settings.setValue( "routeAlphaHighlighted", routingManager->routeColorHighlighted().alpha() );
        settings.setValue( "routeColorAlternative", routingManager->routeColorAlternative().name() );
        settings.setValue( "routeAlphaAlternative", routingManager->routeColorAlternative().alpha() );
    }
    settings.endGroup();

    m_marbleWidget->writePluginSettings( settings );
}

void MainWindow::showDownloadRegionDialog()
{
    if ( !m_downloadRegionDialog ) {
        m_downloadRegionDialog = new DownloadRegionDialog( m_marbleWidget, this );
        // it might be tempting to move the connects to DownloadRegionDialog's "accepted" and
        // "applied" signals, be aware that the "hidden" signal might be come before the "accepted"
        // signal, leading to a too early disconnect.
        connect( m_downloadRegionDialog, SIGNAL(accepted()), SLOT(downloadRegion()));
        connect( m_downloadRegionDialog, SIGNAL(applied()), SLOT(downloadRegion()));
    }
    // FIXME: get allowed range from current map theme
    m_downloadRegionDialog->setAllowedTileLevelRange( 0, 16 );
    m_downloadRegionDialog->setSelectionMethod( DownloadRegionDialog::VisibleRegionMethod );
    ViewportParams const * const viewport = m_marbleWidget->viewport();
    m_downloadRegionDialog->setSpecifiedLatLonAltBox( viewport->viewLatLonAltBox() );
    m_downloadRegionDialog->setVisibleLatLonAltBox( viewport->viewLatLonAltBox() );

    m_downloadRegionDialog->show();
    m_downloadRegionDialog->raise();
    m_downloadRegionDialog->activateWindow();
}

void MainWindow::downloadRegion()
{
    Q_ASSERT( m_downloadRegionDialog );
    QVector<TileCoordsPyramid> const pyramid = m_downloadRegionDialog->region();
    if ( !pyramid.isEmpty() ) {
        m_marbleWidget->downloadRegion( pyramid );
    }
}

void MainWindow::showMapViewDialog()
{
    if( !m_mapViewDialog ) {
        m_mapViewDialog = new QDialog( this );
        m_mapViewDialog->setWindowTitle( tr( "Map View - Marble" ) );

        QVBoxLayout *layout = new QVBoxLayout;
        m_mapViewDialog->setLayout( layout );

        MapViewWidget *mapViewWidget = new MapViewWidget( m_mapViewDialog );
        mapViewWidget->setMarbleWidget( m_marbleWidget, &m_mapThemeManager );
        layout->addWidget( mapViewWidget );
    }

    m_mapViewDialog->show();
    m_mapViewDialog->raise();
    m_mapViewDialog->activateWindow();
}

void MainWindow::showRoutingDialog()
{
    if( !m_routingWindow ) {
        m_routingWindow = new StackableWindow( this );
        m_routingWindow->setWindowTitle( tr( "Routing - Marble" ) );

        m_routingWidget = new RoutingWidget( m_marbleWidget, m_routingWindow );
        m_routingWidget->setShowDirectionsButtonVisible( true );

        QScrollArea* scrollArea = new QScrollArea;
        m_routingWidget->setMinimumWidth( 760 );
        scrollArea->setWidgetResizable( true );
        scrollArea->setWidget( m_routingWidget );

        QAction *openAction = new QAction( tr( "Open Route..." ), this );
        connect( openAction, SIGNAL(triggered()), m_routingWidget, SLOT(openRoute()) );
        m_routingWindow->menuBar()->addAction( openAction );

        QAction* saveAction = new QAction( tr( "Save Route..." ), this );
        connect( saveAction, SIGNAL(triggered()), m_routingWidget, SLOT(saveRoute()) );
        m_routingWindow->menuBar()->addAction( saveAction );

        m_routingWindow->setCentralWidget( scrollArea );
    }

    m_routingWindow->show();
    m_routingWindow->raise();
    m_routingWindow->activateWindow();
}

void MainWindow::showTrackingDialog()
{
    initializeTrackingWidget();
    m_trackingWindow->show();
    m_trackingWindow->raise();
    m_trackingWindow->activateWindow();
}

void MainWindow::initializeTrackingWidget()
{
    if( !m_trackingWindow ) {
        m_trackingWindow = new StackableWindow( this );
        m_trackingWindow->setWindowTitle( tr( "Tracking - Marble" ) );
        CurrentLocationWidget *trackingWidget = new CurrentLocationWidget( m_trackingWindow );
        trackingWidget->setMarbleWidget( m_marbleWidget );

        m_trackingWindow->setCentralWidget( trackingWidget );
    }
}

void MainWindow::showGoToDialog()
{
    if ( !m_gotoDialog ) {
        m_gotoDialog = new GoToDialog( m_marbleWidget->model(), this );
    }

    m_gotoDialog->show();
    if ( m_gotoDialog->exec() == QDialog::Accepted ) {
        const GeoDataCoordinates coordinates = m_gotoDialog->coordinates();
        m_marbleWidget->centerOn( coordinates );
    }
}

void MainWindow::fallBackToDefaultTheme()
{
    if ( !m_mapThemeManager.mapThemeIds().contains( m_marbleWidget->mapThemeId() ) ) {
        m_marbleWidget->setMapThemeId( defaultMapThemeId );
    }
}

#include "MobileMainWindow.moc"
