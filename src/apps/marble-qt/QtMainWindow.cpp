//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2010 Torsten Rahn        <tackat@kde.org>
// Copyright 2007      Inge Wallin         <ingwa@kde.org>
// Copyright 2011-2013 Bernhard Beschow    <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Illya Kovalevskyy   <illya.kovalevskyy@gmail.com>
// Copyright 2012      Mohammed Nafees     <nafees.technocool@gmail.com>
//

#include "QtMainWindow.h"

#include "MarbleDebug.h"
#include <QList>
#include <QSettings>
#include <QUrl>
#include <QCloseEvent>
#include <QVariant>
#include <QVector>
#include <QAction>
#include <QLabel>
#include <QWhatsThis>
#include <QApplication>
#include <QIcon>
#include <QMenuBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QToolBar>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QDialogButtonBox>
#include <QClipboard>
#include <QNetworkProxy>

#include "EditBookmarkDialog.h"
#include "BookmarkManagerDialog.h"
#include "CurrentLocationWidget.h"
#include "MapViewWidget.h"
#include "MarbleDirs.h"
#include "MarbleAboutDialog.h"
#include "QtMarbleConfigDialog.h"
#include "SunControlWidget.h"
#include "TimeControlWidget.h"
#include "MarbleLocale.h"
#include "DownloadRegionDialog.h"
#include "ViewportParams.h"
#include "AbstractFloatItem.h"
#include "MarbleModel.h"
#include "MarbleClock.h"
#include "HttpDownloadManager.h"
#include "BookmarkManager.h"
#include "NewBookmarkFolderDialog.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoDataLookAt.h"
#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoUriParser.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingProfilesModel.h"
#include "routing/RoutingWidget.h"
#include "routing/RouteRequest.h"
#include "ParseRunnerPlugin.h"
#include "PositionTracking.h"
#include "PositionProviderPlugin.h"
#include "PluginManager.h"
#include "MapThemeDownloadDialog.h"
#include "MapWizard.h"
#include "MarbleWidgetInputHandler.h"
#include "Planet.h"
#include "cloudsync/CloudSyncManager.h"
#include "cloudsync/BookmarkSyncManager.h"
#include "cloudsync/RouteSyncManager.h"
#include "MovieCaptureDialog.h"
#include "DataMigration.h"
#include "TileCoordsPyramid.h"

using namespace Marble;
/* TRANSLATOR Marble::MainWindow */

MainWindow::MainWindow(const QString& marbleDataPath, const QVariantMap& cmdLineSettings, QWidget *parent) :
        QMainWindow(parent),
        m_controlView( 0 ),
        m_savedSize( QSize(-1, -1) ),
        m_sunControlDialog( 0 ),
        m_timeControlDialog( 0 ),
        m_configDialog( 0 ),
        m_downloadRegionDialog( 0 ),
        m_movieCaptureDialog( 0 ),

        // File Menu
        m_fileMenu( 0 ),
        m_viewMenu( 0 ),
        m_helpMenu( 0 ),
        m_settingsMenu( 0 ),
        m_panelMenu( 0 ),
        m_viewSizeMenu( 0 ),
        m_infoBoxesMenu( 0 ),
        m_onlineServicesMenu( 0 ),
        m_bookmarkMenu( 0 ),
        m_openAction( 0 ),
        m_exportMapAction( 0 ),
        m_downloadAction( 0 ),
        m_downloadRegionAction( 0 ),
        m_printPreviewAction( 0 ),
        m_printAction( 0 ),
        m_workOfflineAction( 0 ),
        m_quitAction( 0 ),
        m_mapWizardAction( 0 ),

        // Edit Menu
        m_copyMapAction( 0 ),
        m_copyCoordinatesAction( 0 ),
        m_osmEditAction( 0 ),
        m_recordMovieAction( 0 ),
        m_stopRecordingAction( 0 ),

        // View Menu
        m_showCloudsAction( 0 ),\
        m_controlSunAction( 0 ),
        m_controlTimeAction( 0 ),
        m_reloadAction( 0 ),

        // Settings Menu
        m_fullScreenAction( 0 ),
        m_statusBarAction( 0 ),
        m_configDialogAction( 0 ),
        m_viewSizeActsGroup( 0 ),

        // Help Menu
        m_whatsThisAction( 0 ),
        m_aboutMarbleAction( 0 ),
        m_aboutQtAction( 0 ),
        m_lockFloatItemsAction( 0 ),
        m_handbookAction( 0 ),
        m_forumAction( 0 ),

        // Status Bar
        m_positionLabel( 0 ),
        m_distanceLabel( 0 ),
        m_zoomLabel( 0 ),
        m_clockLabel( 0 ),
        m_downloadProgressBar( 0 ),
        m_toggleTileLevelAction( 0 ),
        m_angleDisplayUnitActionGroup( 0 ),
        m_dmsDegreeAction( 0 ),
        m_decimalDegreeAction( 0 ),
        m_utmAction( 0 ),

        //Bookmark Menu
        m_addBookmarkAction( 0 ),
        m_setHomeAction( 0 ),
        m_toggleBookmarkDisplayAction( 0 ),
        m_manageBookmarksAction( 0 )
{
    setUpdatesEnabled( false );

    QString selectedPath = marbleDataPath.isEmpty() ? readMarbleDataPath() : marbleDataPath;
    if ( !selectedPath.isEmpty() )
        MarbleDirs::setMarbleDataPath( selectedPath );

#ifdef Q_OS_WIN
	QPointer<DataMigration> migration = new DataMigration(this);
	migration->exec();
#endif

    m_controlView = new ControlView( this );

    setWindowIcon(QIcon(QStringLiteral(":/icons/marble.png")));
    setCentralWidget( m_controlView );

    // Initializing config dialog
    m_configDialog = new QtMarbleConfigDialog( m_controlView->marbleWidget(),
                                               m_controlView->cloudSyncManager(),
                                               this );
    connect( m_configDialog, SIGNAL(settingsChanged()),
             this, SLOT(updateSettings()) );
    connect( m_configDialog, SIGNAL(clearVolatileCacheClicked()),
             m_controlView->marbleWidget(), SLOT(clearVolatileTileCache()) );
    connect( m_configDialog, SIGNAL(clearPersistentCacheClicked()),
             m_controlView->marbleModel(), SLOT(clearPersistentTileCache()) );
    connect( m_configDialog, SIGNAL(syncNowClicked()),
             m_controlView->cloudSyncManager()->bookmarkSyncManager(), SLOT(startBookmarkSync()) );
    connect(m_configDialog, SIGNAL(syncNowClicked()),
            m_configDialog, SLOT(disableSyncNow()));

    // Load bookmark file. If it does not exist, a default one will be used.
    m_controlView->marbleModel()->bookmarkManager()->loadFile( "bookmarks/bookmarks.kml" );

    createActions();
    QList<QAction*> const panelActions = m_controlView->setupDockWidgets( this );
    createMenus( panelActions );
    createStatusBar();

    connect( m_controlView->marbleWidget(), SIGNAL(themeChanged(QString)),
             this, SLOT(updateMapEditButtonVisibility(QString)) );
    connect(m_controlView->marbleModel(), SIGNAL(themeChanged(QString)),
            this, SLOT(updateWindowTitle()));
    connect( m_controlView, SIGNAL(showMapWizard()), this, SLOT(showMapWizard()) );
    connect( m_controlView, SIGNAL(mapThemeDeleted()), this, SLOT(fallBackToDefaultTheme()) );

    updateWindowTitle();
    setUpdatesEnabled( true );

    m_position =  QCoreApplication::translate( "Marble", NOT_AVAILABLE );
    m_distance = marbleWidget()->distanceString();
    m_zoom = QString::number( marbleWidget()->tileZoomLevel() );

    m_clock = QLocale().toString( m_controlView->marbleModel()->clockDateTime().addSecs( m_controlView->marbleModel()->clockTimezone() ), QLocale::ShortFormat );
    QMetaObject::invokeMethod(this,
                              "initObject", Qt::QueuedConnection,
                              Q_ARG(QVariantMap, cmdLineSettings));
}

MainWindow::~MainWindow()
{
    delete m_movieCaptureDialog;
}

void MainWindow::addGeoDataFile( const QString &fileName )
{
    QFileInfo file( fileName );

    if ( !file.exists() )
        return;

    // delay file loading to initObject(), such that restoring view from previous session in readSettings()
    // doesn't interfere with focusing on these files
    m_commandlineFilePaths << file.absoluteFilePath();
}

void MainWindow::initObject(const QVariantMap& cmdLineSettings)
{
    QCoreApplication::processEvents ();
    setupStatusBar();
    readSettings(cmdLineSettings);

    foreach ( const QString &path, m_commandlineFilePaths ) {
        m_controlView->marbleModel()->addGeoDataFile( path );
    }

    if ( cmdLineSettings.contains( "tour" ) ) {
        QString const tour = cmdLineSettings.value( "tour" ).toString();
        m_controlView->openTour( tour );
    }
    m_commandlineFilePaths.clear();
}

void MainWindow::createActions()
 {
     m_openAction = new QAction(QIcon(QStringLiteral(":/icons/document-open.png")), tr("&Open..."), this);
     m_openAction->setShortcut( tr( "Ctrl+O" ) );
     m_openAction->setStatusTip( tr( "Open a file for viewing on Marble"));
     connect( m_openAction, SIGNAL(triggered()),
              this, SLOT(openFile()) );

     m_downloadAction = new QAction(QIcon(QStringLiteral(":/icons/get-hot-new-stuff.png")), tr("&Download Maps..."), this);
     connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(openMapDialog()));

     m_exportMapAction = new QAction(QIcon(QStringLiteral(":/icons/document-save-as.png")), tr("&Export Map..."), this);
     m_exportMapAction->setShortcut(tr("Ctrl+S"));
     m_exportMapAction->setStatusTip(tr("Save a screenshot of the map"));
     connect(m_exportMapAction, SIGNAL(triggered()), this, SLOT(exportMapScreenShot()));

     // Action: Download Region
     m_downloadRegionAction = new QAction( tr( "Download &Region..." ), this );
     m_downloadRegionAction->setStatusTip( tr( "Download a map region in different zoom levels for offline usage" ) );
     connect( m_downloadRegionAction, SIGNAL(triggered()), SLOT(showDownloadRegionDialog()) );

     m_printAction = new QAction(QIcon(QStringLiteral(":/icons/document-print.png")), tr("&Print..."), this);
     m_printAction->setShortcut(tr("Ctrl+P"));
     m_printAction->setStatusTip(tr("Print a screenshot of the map"));
     connect(m_printAction, SIGNAL(triggered()), this, SLOT(printMapScreenShot()));

     m_printPreviewAction = new QAction(QIcon(QStringLiteral(":/icons/document-print-preview.png")), tr("Print Previe&w ..."), this);
     m_printPreviewAction->setStatusTip(tr("Print a screenshot of the map"));
     connect(m_printPreviewAction, SIGNAL(triggered()), m_controlView, SLOT(printPreview()));

     m_quitAction = new QAction(QIcon(QStringLiteral(":/icons/application-exit.png")), tr("&Quit"), this);
     m_quitAction->setShortcut(tr("Ctrl+Q"));
     m_quitAction->setStatusTip(tr("Quit the Application"));
     connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));

     m_copyMapAction = new QAction(QIcon(QStringLiteral(":/icons/edit-copy.png")), tr("&Copy Map"), this);
     m_copyMapAction->setShortcut(tr("Ctrl+C"));
     m_copyMapAction->setStatusTip(tr("Copy a screenshot of the map"));
     connect(m_copyMapAction, SIGNAL(triggered()), this, SLOT(copyMap()));

     m_osmEditAction = new QAction(QIcon(QStringLiteral(":/icons/edit-map.png")), tr("&Edit Map"), this );
     m_osmEditAction->setShortcut(tr( "Ctrl+E" ) );
     m_osmEditAction->setStatusTip(tr( "Edit the current map region in an external editor" ) );
     updateMapEditButtonVisibility( m_controlView->marbleWidget()->mapThemeId() );
     connect( m_osmEditAction, SIGNAL(triggered()), m_controlView, SLOT(launchExternalMapEditor()) );

     m_recordMovieAction = new QAction(tr("&Record Movie"), this);
     m_recordMovieAction->setStatusTip(tr("Records a movie of the globe"));
     m_recordMovieAction->setShortcut(QKeySequence("Ctrl+Shift+R"));
     m_recordMovieAction->setIcon(QIcon(QStringLiteral(":/icons/animator.png")));
     connect(m_recordMovieAction, SIGNAL(triggered()),
             this, SLOT(showMovieCaptureDialog()));

     m_stopRecordingAction = new QAction( tr("&Stop Recording"), this );
     m_stopRecordingAction->setStatusTip( tr("Stop recording a movie of the globe") );
     m_stopRecordingAction->setShortcut(QKeySequence( "Ctrl+Shift+S" ));
     m_stopRecordingAction->setEnabled( false );
     connect( m_stopRecordingAction, SIGNAL(triggered()),
             this, SLOT(stopRecording()) );

     m_configDialogAction = new QAction(QIcon(QStringLiteral(":/icons/settings-configure.png")), tr("&Configure Marble"), this);
     m_configDialogAction->setStatusTip(tr("Show the configuration dialog"));
     connect(m_configDialogAction, SIGNAL(triggered()), this, SLOT(editSettings()));

     m_copyCoordinatesAction = new QAction(QIcon(QStringLiteral(":/icons/copy-coordinates.png")), tr("C&opy Coordinates"), this);
     m_copyCoordinatesAction->setStatusTip(tr("Copy the center coordinates as text"));
     connect(m_copyCoordinatesAction, SIGNAL(triggered()), this, SLOT(copyCoordinates()));

     m_fullScreenAction = new QAction(QIcon(QStringLiteral(":/icons/view-fullscreen.png")), tr("&Full Screen Mode"), this);
     m_fullScreenAction->setShortcut(tr("Ctrl+Shift+F"));
     m_fullScreenAction->setCheckable( true );
     m_fullScreenAction->setStatusTip(tr("Full Screen Mode"));
     connect(m_fullScreenAction, SIGNAL(triggered(bool)), this, SLOT(showFullScreen(bool)));

     m_statusBarAction = new QAction( tr("&Show Status Bar"), this);
     m_statusBarAction->setCheckable( true );
     m_statusBarAction->setStatusTip(tr("Show Status Bar"));
     connect(m_statusBarAction, SIGNAL(triggered(bool)), this, SLOT(showStatusBar(bool)));


     m_lockFloatItemsAction = new QAction(QIcon(QStringLiteral(":/icons/unlock.png")), tr("Lock Position"), this);
     m_lockFloatItemsAction->setCheckable( true );
     m_lockFloatItemsAction->setStatusTip(tr("Lock Position of Floating Items"));
     connect(m_lockFloatItemsAction, SIGNAL(triggered(bool)), this, SLOT(lockPosition(bool)));

     m_showCloudsAction = new QAction(QIcon(QStringLiteral(":/icons/clouds.png")), tr("&Clouds"), this);
     m_showCloudsAction->setCheckable( true );
     m_showCloudsAction->setStatusTip(tr("Show Real Time Cloud Cover"));
     connect(m_showCloudsAction, SIGNAL(triggered(bool)), this, SLOT(showClouds(bool)));

     m_workOfflineAction = new QAction(QIcon(QStringLiteral(":/icons/user-offline.png")), tr("Work Off&line"), this);
     m_workOfflineAction->setCheckable( true );
     connect(m_workOfflineAction, SIGNAL(triggered(bool)), this, SLOT(workOffline(bool)));

     m_controlTimeAction = new QAction(QIcon(QStringLiteral(":/icons/clock.png")), tr("&Time Control..."), this );
     m_controlTimeAction->setStatusTip( tr( "Configure Time Control " ) );
     connect( m_controlTimeAction, SIGNAL(triggered()), this, SLOT(controlTime()) );

     m_controlSunAction = new QAction( tr( "S&un Control..." ), this );
     m_controlSunAction->setStatusTip( tr( "Configure Sun Control" ) );
     connect( m_controlSunAction, SIGNAL(triggered()), this, SLOT(controlSun()) );

     m_reloadAction = new QAction(QIcon(QStringLiteral(":/icons/view-refresh.png")), tr("&Redisplay"), this);
     m_reloadAction->setShortcut(tr("F5"));
     m_reloadAction->setStatusTip(tr("Reload Current Map"));
     connect(m_reloadAction, SIGNAL(triggered()), this, SLOT(reload()));

     m_handbookAction = new QAction(QIcon(QStringLiteral(":/icons/help-contents.png")), tr("Marble Virtual Globe &Handbook"), this);
     m_handbookAction->setShortcut(tr("F1"));
     m_handbookAction->setStatusTip(tr("Show the Handbook for Marble Virtual Globe"));
     connect(m_handbookAction, SIGNAL(triggered()), this, SLOT(handbook()));

     m_whatsThisAction = new QAction(QIcon(QStringLiteral(":/icons/help-whatsthis.png")), tr("What's &This"), this);
     m_whatsThisAction->setShortcut(tr("Shift+F1"));
     m_whatsThisAction->setStatusTip(tr("Show a detailed explanation of the action."));
     connect(m_whatsThisAction, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

     m_forumAction = new QAction( tr("&Community Forum"), this);
     m_forumAction->setStatusTip(tr("Visit Marble's Community Forum"));
     connect(m_forumAction, SIGNAL(triggered()), this, SLOT(openForum()));

     m_aboutMarbleAction = new QAction(QIcon(QStringLiteral(":/icons/marble.png")), tr("&About Marble Virtual Globe"), this);
     m_aboutMarbleAction->setStatusTip(tr("Show the application's About Box"));
     connect(m_aboutMarbleAction, SIGNAL(triggered()), this, SLOT(aboutMarble()));

     m_aboutQtAction = new QAction(tr("About &Qt"), this);
     m_aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
     connect(m_aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

     //Bookmark Actions
     m_addBookmarkAction = new QAction(QIcon(QStringLiteral(":/icons/bookmark-new.png")), tr("&Add Bookmark"), this);
     m_addBookmarkAction->setShortcut(tr("Ctrl+B"));
     m_addBookmarkAction->setStatusTip(tr("Add Bookmark"));
     connect( m_addBookmarkAction, SIGNAL(triggered()), this, SLOT(openEditBookmarkDialog()) );

     m_setHomeAction = new QAction(QIcon(QStringLiteral(":/icons/go-home.png")), tr("&Set Home Location"), this);
     m_setHomeAction->setStatusTip( tr( "&Set Home Location" ) );
     connect( m_setHomeAction, SIGNAL(triggered()), this, SLOT(setHome()) );

     m_toggleBookmarkDisplayAction = new QAction(tr( "Show &Bookmarks" ), this);
     m_toggleBookmarkDisplayAction->setStatusTip( tr( "Toggle display of Bookmarks" ) );
     m_toggleBookmarkDisplayAction->setCheckable( true );
     connect( m_toggleBookmarkDisplayAction, SIGNAL(triggered(bool)), this, SLOT(showBookmarks(bool)) );

     m_manageBookmarksAction = new QAction(QIcon(QStringLiteral(":/icons/bookmarks-organize.png")), tr("&Manage Bookmarks"), this);
     m_manageBookmarksAction->setStatusTip( tr( "Manage Bookmarks" ) );
     connect( m_manageBookmarksAction, SIGNAL(triggered()), this, SLOT(manageBookmarks()) );
     
     // Map Wizard action
     m_mapWizardAction = new QAction(QIcon(QStringLiteral(":/icons/create-new-map.png")), tr("&Create a New Map..."), this);
     m_mapWizardAction->setStatusTip( tr( "A wizard guides you through the creation of your own map theme." ) );
     connect( m_mapWizardAction, SIGNAL(triggered()), SLOT(showMapWizard()) );

     // Statusbar Actions
     m_toggleTileLevelAction = new QAction( tr( "Show Tile Zoom Level" ), statusBar() );
     m_toggleTileLevelAction->setCheckable( true );
     m_toggleTileLevelAction->setChecked( false );
     connect( m_toggleTileLevelAction, SIGNAL(triggered(bool)),
              this, SLOT(showZoomLevel(bool)) );

     m_angleDisplayUnitActionGroup = new QActionGroup( statusBar() );

     m_dmsDegreeAction = new QAction( tr( "Degree (DMS)" ), statusBar() );
     m_dmsDegreeAction->setCheckable( true );
     m_dmsDegreeAction->setData( (int)DMSDegree );
     m_angleDisplayUnitActionGroup->addAction(m_dmsDegreeAction);

     m_decimalDegreeAction = new QAction( tr( "Degree (Decimal)" ), statusBar() );
     m_decimalDegreeAction->setCheckable( true );
     m_decimalDegreeAction->setData( (int)DecimalDegree );
     m_angleDisplayUnitActionGroup->addAction(m_decimalDegreeAction);

     m_utmAction = new QAction( tr( "Universal Transverse Mercator (UTM)" ), statusBar() );
     m_utmAction->setCheckable( true );
     m_utmAction->setData( (int)UTM );
     m_angleDisplayUnitActionGroup->addAction(m_utmAction);

     connect( m_angleDisplayUnitActionGroup, SIGNAL(triggered(QAction*)),
              this, SLOT(changeAngleDisplayUnit(QAction*)) );

     // View size actions
     m_viewSizeActsGroup = ControlView::createViewSizeActionGroup( this );
     connect( m_viewSizeActsGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeViewSize(QAction*)) );
}

void MainWindow::createMenus( const QList<QAction*> &panelActions )
{
        m_fileMenu = menuBar()->addMenu(tr("&File"));
        m_fileMenu->addAction(m_openAction);
        m_fileMenu->addAction(m_downloadAction);
        m_fileMenu->addAction( m_downloadRegionAction );
        m_fileMenu->addAction( m_mapWizardAction );
        m_fileMenu->addAction(m_exportMapAction);
        m_fileMenu->addSeparator();
        m_fileMenu->addAction(m_printAction);
        m_fileMenu->addAction(m_printPreviewAction);
        m_fileMenu->addSeparator();
        m_fileMenu->addAction(m_workOfflineAction);
        m_fileMenu->addAction(m_quitAction);

        m_fileMenu = menuBar()->addMenu(tr("&Edit"));
        m_fileMenu->addAction(m_copyMapAction);
        m_fileMenu->addAction(m_copyCoordinatesAction);
        m_fileMenu->addAction( m_osmEditAction );
        m_fileMenu->addSeparator();
        m_fileMenu->addAction(m_recordMovieAction);
        m_fileMenu->addAction(m_stopRecordingAction);

        m_viewMenu = menuBar()->addMenu(tr("&View"));
        m_infoBoxesMenu = new QMenu(tr("&Info Boxes"));
        m_onlineServicesMenu = new QMenu(tr("&Online Services"));
        createPluginsMenus();

        m_bookmarkMenu = menuBar()->addMenu(tr("&Bookmarks"));
        createBookmarkMenu();
        connect( m_bookmarkMenu, SIGNAL(aboutToShow()), this, SLOT(createBookmarkMenu()) );

        m_panelMenu = new QMenu(tr("&Panels"));
        foreach( QAction* action, panelActions ) {
            m_panelMenu->addAction( action );
        }

        m_viewSizeMenu = new QMenu(tr("&View Size"));
        m_viewSizeMenu->addActions( m_viewSizeActsGroup->actions() );

        m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
        m_settingsMenu->addMenu( m_panelMenu );
        m_settingsMenu->addAction(m_statusBarAction);
        m_settingsMenu->addSeparator();
        m_settingsMenu->addMenu( m_viewSizeMenu );
        m_settingsMenu->addAction(m_fullScreenAction);
        m_settingsMenu->addSeparator();
        m_settingsMenu->addAction(m_configDialogAction);

        m_helpMenu = menuBar()->addMenu(tr("&Help"));
        m_helpMenu->addAction(m_handbookAction);
        m_helpMenu->addAction(m_forumAction);
        m_helpMenu->addSeparator();
        m_helpMenu->addAction(m_whatsThisAction);
        m_helpMenu->addSeparator();
        m_helpMenu->addAction(m_aboutMarbleAction);
        m_helpMenu->addAction(m_aboutQtAction);

        // FIXME: Discuss if this is the best place to put this
        QList<RenderPlugin *> pluginList = m_controlView->marbleWidget()->renderPlugins();
        QList<RenderPlugin *>::const_iterator it = pluginList.constBegin();
        QList<RenderPlugin *>::const_iterator const listEnd = pluginList.constEnd();
        for (; it != listEnd; ++it ) {
            connect( (*it), SIGNAL(actionGroupsChanged()),
                     this, SLOT(createPluginMenus()) );
        }
}

void MainWindow::createPluginsMenus()
{
    m_onlineServicesMenu->clear();
    m_infoBoxesMenu->clear();
    m_viewMenu->clear();

    m_viewMenu->addAction(m_reloadAction);
    m_viewMenu->addSeparator();

    // Do not create too many menu entries on a MID
    // FIXME: Set up another way of switching the plugins on and off.
    if( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        return;
    }

    m_infoBoxesMenu->addAction(m_lockFloatItemsAction);
    m_infoBoxesMenu->addSeparator();
    QList<QAction*> themeActions;

    QList<RenderPlugin *> renderPluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = renderPluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = renderPluginList.constEnd();
    for (; i != end; ++i ) {
        switch( (*i)->renderType() ) {
        case RenderPlugin::TopLevelRenderType:
            m_viewMenu->addAction( (*i)->action() );
            break;
        case RenderPlugin::PanelRenderType:
            m_infoBoxesMenu->addAction( (*i)->action() );
            break;
        case RenderPlugin::OnlineRenderType:
            m_onlineServicesMenu->addAction( (*i)->action() );
            break;
        case RenderPlugin::ThemeRenderType:
            themeActions.append( (*i)->action() );
            break;
        default:
            mDebug() << "No menu entry created for plugin with unknown render type:" << (*i)->nameId();
            break;
        }
    }
    m_viewMenu->addMenu( m_infoBoxesMenu );
    m_viewMenu->addMenu( m_onlineServicesMenu );
    m_viewMenu->addActions( themeActions );
    m_viewMenu->addAction( m_showCloudsAction );
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_controlSunAction);
    m_viewMenu->addAction(m_controlTimeAction);
}

void MainWindow::createBookmarksListMenu( QMenu *bookmarksListMenu, const GeoDataContainer *container )
{
    //m_bookmarksListMenu->clear();

    QVector<GeoDataPlacemark*> bookmarks = container->placemarkList();

    foreach ( const GeoDataPlacemark *placemark, bookmarks ) {
        QAction *bookmarkAction = new QAction( placemark->name(), this );
        QVariant var;

        const GeoDataLookAt* lookAt = placemark->lookAt();
        if ( !lookAt ) {
            GeoDataCoordinates coordinates = placemark->coordinate();
            GeoDataLookAt coordinateToLookAt;
            coordinateToLookAt.setCoordinates( coordinates );
            coordinateToLookAt.setRange( marbleWidget()->lookAt().range() );
            var.setValue( coordinateToLookAt );
        } else {
            var.setValue( *lookAt );
        }
        bookmarkAction->setData( var );
        bookmarksListMenu->addAction( bookmarkAction );
    }

}
void MainWindow::createBookmarkMenu()
{
    m_bookmarkMenu->clear();
    m_bookmarkMenu->addAction( m_addBookmarkAction );
    m_bookmarkMenu->addAction( m_toggleBookmarkDisplayAction );
    m_toggleBookmarkDisplayAction->setChecked( m_controlView->marbleModel()->bookmarkManager()->document()->isVisible() );
    m_bookmarkMenu->addAction( m_setHomeAction );
    m_bookmarkMenu->addAction( m_manageBookmarksAction );

    m_bookmarkMenu->addSeparator();

    m_bookmarkMenu->addAction( QIcon(QStringLiteral(":/icons/go-home.png")), tr("&Home"),
                               m_controlView->marbleWidget(), SLOT(goHome()) );
    createFolderList( m_bookmarkMenu, m_controlView->marbleModel()->bookmarkManager()->document() );
}

void MainWindow::createFolderList( QMenu *bookmarksListMenu, const GeoDataContainer *container )
{
    QVector<GeoDataFolder*> folders = container->folderList();

    if ( folders.size() == 1 ) {
        createBookmarksListMenu( bookmarksListMenu, folders.first() );
    }
    else {
        foreach ( const GeoDataFolder *folder, folders ) {
            QMenu *subMenu = bookmarksListMenu->addMenu(QIcon(QStringLiteral(":/icons/folder-bookmark.png")), folder->name());
            createFolderList( subMenu, folder );
            connect( subMenu, SIGNAL(triggered(QAction*)),
                                      this, SLOT(lookAtBookmark(QAction*)) );
        }
    }

    createBookmarksListMenu( bookmarksListMenu, container );
    connect( bookmarksListMenu, SIGNAL(triggered(QAction*)),
                              this, SLOT(lookAtBookmark(QAction*)) );
}


void MainWindow::lookAtBookmark( QAction *action)
{
    if ( action->data().isNull() ) {
        return;
    }

    GeoDataLookAt temp = qvariant_cast<GeoDataLookAt>( action->data() ) ;
    m_controlView->marbleWidget()->flyTo( temp ) ;
    mDebug() << " looking at bookmark having longitude : "<< temp.longitude(GeoDataCoordinates::Degree)
             << " latitude :  "<< temp.latitude(GeoDataCoordinates::Degree)
             << " distance : " << temp.range();


}

void MainWindow::manageBookmarks()
{
    MarbleModel * const model = m_controlView->marbleModel();
    QPointer<BookmarkManagerDialog> dialog = new BookmarkManagerDialog( model, this );
    dialog->exec();
    delete dialog;
}

void MainWindow::setHome()
{
    MarbleWidget *widget = m_controlView->marbleWidget();
    widget->model()->setHome( widget->centerLongitude(), widget->centerLatitude(), widget->zoom() );
}

void MainWindow::openEditBookmarkDialog()
{
    MarbleWidget *widget = m_controlView->marbleWidget();
    QPointer<EditBookmarkDialog> dialog = new EditBookmarkDialog( widget->model()->bookmarkManager(), widget );
    dialog->setMarbleWidget( widget );
    dialog->setCoordinates( widget->lookAt().coordinates() );
    dialog->setRange( widget->lookAt().range() );
    dialog->setReverseGeocodeName();
    if ( dialog->exec() == QDialog::Accepted ) {
        widget->model()->bookmarkManager()->addBookmark( dialog->folder(), dialog->bookmark() );
    }
    delete dialog;
}

void MainWindow::createPluginMenus()
{
    // Remove and delete toolbars if they exist
    while( !m_pluginToolbars.isEmpty() ) {
        QToolBar* tb = m_pluginToolbars.takeFirst();
        this->removeToolBar(tb);
        tb->deleteLater();
    }

    // Do not create too many menu entries on a MID
    // FIXME: Set up another way of switching the plugins on and off.
    if( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        return;
    }

    //remove and delete old menus if they exist
    while(  !m_pluginMenus.isEmpty() ) {
        m_viewMenu->removeAction( m_pluginMenus.takeFirst() );
    }

    QList<RenderPlugin *> renderPluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = renderPluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = renderPluginList.constEnd();

    for (; i != end; ++i ) {

        // menus
        const QList<QActionGroup*> *tmp_actionGroups = (*i)->actionGroups();
        if ((*i)->enabled() && tmp_actionGroups && (*i)->nameId() != QLatin1String("annotation")) {
           foreach( QActionGroup *ag, *tmp_actionGroups ) {
               if( !ag->actions().isEmpty() ) {
                   m_pluginMenus.append( m_viewMenu->addSeparator() );
               }
               foreach( QAction *action, ag->actions() ) {
                   m_viewMenu->addAction( action );
                   m_pluginMenus.append( action );
               }
           }
        }

        // toolbars
        const QList<QActionGroup*> *tmp_toolbarActionGroups = (*i)->toolbarActionGroups();
        if ( (*i)->enabled() && tmp_toolbarActionGroups ) {
            QToolBar* toolbar = new QToolBar(this);
            toolbar->setObjectName(QLatin1String("plugin-toolbar-") + (*i)->nameId());

            foreach( QActionGroup* ag, *tmp_toolbarActionGroups ) {
                toolbar->addActions( ag->actions() );
                if ( tmp_toolbarActionGroups->last() != ag ) {
                    toolbar->addSeparator();
                }
            }

            m_pluginToolbars.append( toolbar );
            this->addToolBar( toolbar );
        }
    }

//    FIXME: load the menus once the method has been settled on
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
    statusBar()->hide();
}

void MainWindow::openMapDialog()
{
    QPointer<MapThemeDownloadDialog> dialog( new MapThemeDownloadDialog( m_controlView->marbleWidget() ) );
    dialog->exec();
    delete dialog;
}

void MainWindow::exportMapScreenShot()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Map"), // krazy:exclude=qclasses
                            QDir::homePath(),
                            tr("Images (*.jpg *.png)"));

    if ( !fileName.isEmpty() )
    {
        // Take the case into account where no file format is indicated
        const char * format = 0;
        if ( !fileName.endsWith(QLatin1String( "png" ), Qt::CaseInsensitive)
           && !fileName.endsWith(QLatin1String( "jpg" ), Qt::CaseInsensitive) )
        {
            format = "JPG";
        }

        QPixmap mapPixmap = m_controlView->mapScreenShot();
        bool success = mapPixmap.save( fileName, format );
        if ( !success )
        {
            QMessageBox::warning(this, tr("Marble"), // krazy:exclude=qclasses
                   tr( "An error occurred while trying to save the file.\n" ),
                   QMessageBox::Ok);
        }
    }
}


void MainWindow::showFullScreen( bool isChecked )
{
    if ( isChecked ) {
        setWindowState( windowState() | Qt::WindowFullScreen ); // set
    }
    else {
        setWindowState( windowState() & ~Qt::WindowFullScreen ); // reset
    }

    m_fullScreenAction->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::copyCoordinates()
{
    qreal lon = m_controlView->marbleWidget()->centerLongitude();
    qreal lat = m_controlView->marbleWidget()->centerLatitude();

    QString  positionString = GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Degree ).toString();
    QClipboard  *clipboard = QApplication::clipboard();

    clipboard->setText( positionString );
}

void MainWindow::copyMap()
{
    QPixmap      mapPixmap = m_controlView->mapScreenShot();
    QClipboard  *clipboard = QApplication::clipboard();

    clipboard->setPixmap( mapPixmap );
}

void MainWindow::showStatusBar( bool isChecked )
{
    if ( isChecked )
    {
        statusBar()->show();
    }
    else
    {
        statusBar()->hide();
    }

    m_statusBarAction->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::showClouds( bool isChecked )
{
    m_controlView->marbleWidget()->setShowClouds( isChecked );

    m_showCloudsAction->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::showBookmarks( bool show )
{
    m_controlView->marbleModel()->bookmarkManager()->setShowBookmarks( show );

    m_toggleBookmarkDisplayAction->setChecked( show ); // Sync state with the GUI
}

void MainWindow::workOffline( bool offline )
{
    m_controlView->setWorkOffline( offline );

    m_workOfflineAction->setChecked( offline ); // Sync state with the GUI
}

void MainWindow::lockPosition( bool isChecked )
{
    QList<AbstractFloatItem *> floatItemList = m_controlView->marbleWidget()->floatItems();

    QList<AbstractFloatItem *>::const_iterator i = floatItemList.constBegin();
    QList<AbstractFloatItem *>::const_iterator const end = floatItemList.constEnd();
    for (; i != end; ++i )
    {
        // Locking one would suffice as it affects all.
        // Nevertheless go through all.
        (*i)->setPositionLocked(isChecked);
    }
}

void MainWindow::controlSun()
{
    if (!m_sunControlDialog) {
        m_sunControlDialog = new SunControlWidget( m_controlView->marbleWidget(), this );
        connect( m_sunControlDialog, SIGNAL(showSun(bool)),
                 this,               SLOT (showSun(bool)) );
    }

     m_sunControlDialog->show();
     m_sunControlDialog->raise();
     m_sunControlDialog->activateWindow();
}

void MainWindow::controlTime()
{
    if ( !m_timeControlDialog )
    {
        m_timeControlDialog = new TimeControlWidget( m_controlView->marbleModel()->clock() );
    }
    /* m_timeControlDialog is a modeless dialog so that user may adjust time and interact with main application simultaneously.*/
    m_timeControlDialog->show();
    m_timeControlDialog->raise();
    m_timeControlDialog->activateWindow();
}


void MainWindow::showSun( bool active )
{
    m_controlView->marbleWidget()->setShowSunShading( active );
}

void MainWindow::reload()
{
    m_controlView->marbleWidget()->reloadMap();
}

void MainWindow::enterWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void MainWindow::aboutMarble()
{
    MarbleAboutDialog dlg(this);
    dlg.setApplicationTitle( tr( "Marble Virtual Globe %1" ).arg( ControlView::applicationVersion() ) );
    dlg.exec();
}

void MainWindow::handbook()
{
    const QString code = MarbleLocale::languageCode();

    QUrl handbookLocation(QLatin1String("http://docs.kde.org/stable/") + code + QLatin1String("/kdeedu/marble/index.html"));

    // TODO: this logic seems broken. Should that check "code.isEmpty()" instead?
    // and how do we konw there is a doc for the code?
    if ( handbookLocation.isEmpty() )
        handbookLocation = QUrl("http://docs.kde.org/stable/en/kdeedu/marble/index.html");

    if( !QDesktopServices::openUrl( handbookLocation ) )
    qDebug() << "URL not opened";
}

void MainWindow::openForum()
{
    QUrl forumLocation("https://forum.kde.org/viewforum.php?f=217");
    if( !QDesktopServices::openUrl( forumLocation ) ) {
        mDebug() << "Failed to open URL " << forumLocation.toString();
    }
}

void MainWindow::showPosition( const QString& position )
{
    m_position = position;
    updateStatusBar();
}

void MainWindow::showDistance( const QString& distance )
{
    m_distance = distance;
    updateStatusBar();
}

void MainWindow::showZoom( int zoom )
{
    m_zoom = QString::number( zoom );
    updateStatusBar();
}

void MainWindow::showDateTime()
{
    m_clock = QLocale().toString( m_controlView->marbleModel()->clockDateTime().addSecs( m_controlView->marbleModel()->clockTimezone() ), QLocale::ShortFormat );
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    if ( m_positionLabel )
        m_positionLabel->setText(tr("Position: %1").arg(m_position));

    if ( m_distanceLabel )
        m_distanceLabel->setText(tr("Altitude: %1").arg(m_distance));

    if ( m_zoomLabel )
        m_zoomLabel->setText(tr("Zoom: %1").arg(m_zoom));

    if ( m_clockLabel )
        m_clockLabel->setText(tr("Time: %1").arg(m_clock));

    switch ( m_configDialog->angleUnit() ) {
    case DMSDegree:
        m_dmsDegreeAction->setChecked( true );
        break;
    case DecimalDegree:
        m_decimalDegreeAction->setChecked( true );
        break;
    case UTM:
        m_utmAction->setChecked( true );
        break;
    }
}

void MainWindow::openFile()
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
    const QString allFileTypes = tr("All Supported Files") + QLatin1String(" (") + allFileExtensions.join(QLatin1Char(' ')) + QLatin1Char(')');

    filters.sort();
    filters.prepend( allFileTypes );
    const QString filter = filters.join( ";;" );

    QStringList fileNames = QFileDialog::getOpenFileNames( this, tr( "Open File" ), m_lastFileOpenPath, filter );

    if ( !fileNames.isEmpty() ) {
        const QString firstFile = fileNames.first();
        m_lastFileOpenPath = QFileInfo( firstFile ).absolutePath();
    }

    foreach( const QString &fileName, fileNames ) {
        m_controlView->marbleModel()->addGeoDataFile( fileName );
    }
}

void MainWindow::setupStatusBar()
{
    statusBar()->setSizeGripEnabled( true );
    statusBar()->setContextMenuPolicy( Qt::ActionsContextMenu );

    statusBar()->addAction( m_toggleTileLevelAction );

    QMenu *angleDisplayUnitMenu = new QMenu( );
    angleDisplayUnitMenu->addActions( m_angleDisplayUnitActionGroup->actions() );
    QAction *angleDisplayUnitMenuAction = new QAction( tr("&Angle Display Unit"), statusBar() );
    angleDisplayUnitMenuAction->setMenu( angleDisplayUnitMenu );
    statusBar()->addAction( angleDisplayUnitMenuAction );

    setupDownloadProgressBar();

    m_positionLabel = new QLabel( );
    m_positionLabel->setIndent( 5 );
    // UTM syntax is used in the template string, as it is longer than the lon/lat one
    QString templatePositionString = tr("Position: %1").arg(QLatin1String(" 00Z 000000.00 m E, 00000000.00 m N_"));
    int maxPositionWidth = fontMetrics().boundingRect(templatePositionString).width()
                            + 2 * m_positionLabel->margin() + 2 * m_positionLabel->indent();
    m_positionLabel->setFixedWidth( maxPositionWidth );
    statusBar()->addPermanentWidget ( m_positionLabel );

    m_distanceLabel = new QLabel( );
    m_distanceLabel->setIndent( 5 );
    QString templateDistanceString = tr("Altitude: %1").arg(QLatin1String(" 00.000,0 mu"));
    int maxDistanceWidth = fontMetrics().boundingRect(templateDistanceString).width()
                            + 2 * m_distanceLabel->margin() + 2 * m_distanceLabel->indent();
    m_distanceLabel->setFixedWidth( maxDistanceWidth );
    statusBar()->addPermanentWidget ( m_distanceLabel );

    m_zoomLabel = new QLabel( );
    m_zoomLabel->setIndent( 5 );
    QString templateZoomString = tr("Zoom: %1").arg(QLatin1String(" 00"));
    int maxZoomWidth = fontMetrics().boundingRect(templateZoomString).width()
                            + 2 * m_zoomLabel->margin() + 2 * m_zoomLabel->indent();
    m_zoomLabel->setFixedWidth( maxZoomWidth );
    // Not added here, but activated by the user with the context menu

    m_clockLabel = new QLabel( );
    m_clockLabel->setIndent( 5 );
    QString templateDateTimeString = tr("Time: %1").arg(QLocale().toString(QDateTime::fromString( "01:01:1000", "dd:mm:yyyy"), QLocale::ShortFormat));
    int maxDateTimeWidth = fontMetrics().boundingRect( templateDateTimeString ).width()
                            + 2 * m_clockLabel->margin() + 2 * m_clockLabel->indent();
    m_clockLabel->setFixedWidth( maxDateTimeWidth );
    statusBar()->addPermanentWidget ( m_clockLabel );

    connect( marbleWidget(), SIGNAL(mouseMoveGeoPosition(QString)),
              this, SLOT(showPosition(QString)) );
    connect( marbleWidget(), SIGNAL(distanceChanged(QString)),
              this, SLOT(showDistance(QString)) );
    connect( marbleWidget(), SIGNAL(tileLevelChanged(int)),
            this, SLOT(showZoom(int)) );
    connect( m_controlView->marbleModel()->clock(), SIGNAL(timeChanged()),
              this, SLOT(showDateTime()) );

    updateStatusBar();
}

void MainWindow::setupDownloadProgressBar()
{
    m_downloadProgressBar = new QProgressBar;
    m_downloadProgressBar->setVisible( true );
    statusBar()->addPermanentWidget( m_downloadProgressBar );

    HttpDownloadManager * const downloadManager =
        m_controlView->marbleModel()->downloadManager();
    Q_ASSERT( downloadManager );
    connect( downloadManager, SIGNAL(progressChanged(int,int)), SLOT(handleProgress(int,int)) );
    connect( downloadManager, SIGNAL(jobRemoved()), SLOT(removeProgressItem()) );
}

void MainWindow::handleProgress( int active, int queued ){
    m_downloadProgressBar->setUpdatesEnabled( false );
    if ( m_downloadProgressBar->value() < 0 ) {
        m_downloadProgressBar->setMaximum( 1 );
        m_downloadProgressBar->setValue( 0 );
        m_downloadProgressBar->setVisible( true );
    } else {
        m_downloadProgressBar->setMaximum( qMax<int>( m_downloadProgressBar->maximum(), active + queued ) );
    }

    m_downloadProgressBar->setUpdatesEnabled( true );
}

void MainWindow::removeProgressItem(){
    m_downloadProgressBar->setUpdatesEnabled( false );
    m_downloadProgressBar->setValue( m_downloadProgressBar->value() + 1 );
    if ( m_downloadProgressBar->value() == m_downloadProgressBar->maximum() ) {
        m_downloadProgressBar->reset();
        m_downloadProgressBar->setVisible( false );
    }

    m_downloadProgressBar->setUpdatesEnabled( true );
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    writeSettings();

    QCloseEvent newEvent;
    QCoreApplication::sendEvent( m_controlView, &newEvent );

    if ( newEvent.isAccepted() ) {
        event->accept();
    } else {
        event->ignore();
    }
}

QString MainWindow::readMarbleDataPath()
{
     QSettings settings;

     settings.beginGroup("MarbleWidget");
         QString marbleDataPath;
         marbleDataPath = settings.value("marbleDataPath", "").toString(),
     settings.endGroup();

     return marbleDataPath;
}

void MainWindow::readSettings(const QVariantMap& overrideSettings)
{
     QSettings settings;

     settings.beginGroup("MainWindow");
         resize(settings.value("size", QSize(640, 480)).toSize());
         move(settings.value("pos", QPoint(200, 200)).toPoint());
         showFullScreen(settings.value("fullScreen", false ).toBool());
         showStatusBar(settings.value("statusBar", false ).toBool());
         showZoomLevel(settings.value("showZoomLevel",false).toBool());
         show();
         showClouds(settings.value("showClouds", true ).toBool());
         workOffline(settings.value("workOffline", false ).toBool());
         m_controlView->marbleWidget()->setShowAtmosphere(settings.value("showAtmosphere", true ).toBool());
         m_lastFileOpenPath = settings.value("lastFileOpenDir", QDir::homePath()).toString();
         showBookmarks( settings.value( "showBookmarks", true ).toBool() );
         restoreState( settings.value("windowState").toByteArray() );
     settings.endGroup();

     setUpdatesEnabled(false);

     settings.beginGroup("MarbleWidget");
         QString mapThemeId;
         const QVariantMap::ConstIterator mapThemeIdIt = overrideSettings.find(QLatin1String("mapTheme"));
         if ( mapThemeIdIt != overrideSettings.constEnd() ) {
            mapThemeId = mapThemeIdIt.value().toString();
         } else {
            mapThemeId = settings.value("mapTheme", m_controlView->defaultMapThemeId() ).toString();
         }
         mDebug() << Q_FUNC_INFO << "mapThemeId:" << mapThemeId;
         m_controlView->marbleWidget()->setMapThemeId( mapThemeId );
         m_controlView->marbleWidget()->setProjection(
            (Projection)(settings.value("projection", Spherical ).toInt())
         );

         // Set home position
         m_controlView->marbleModel()->setHome(
            settings.value("homeLongitude", 9.4).toDouble(),
            settings.value("homeLatitude", 54.8).toDouble(),
            settings.value("homeZoom", 1050 ).toInt()
         );

         // Center on/Distance
         const QVariantMap::ConstIterator distanceIt = overrideSettings.find(QLatin1String("distance"));
         const bool isDistanceOverwritten = (distanceIt != overrideSettings.constEnd());

         const QVariantMap::ConstIterator lonLatIt = overrideSettings.find(QLatin1String("lonlat"));
         if ( lonLatIt != overrideSettings.constEnd() ) {
            const QVariantList lonLat = lonLatIt.value().toList();
            m_controlView->marbleWidget()->centerOn( lonLat.at(0).toDouble(), lonLat.at(1).toDouble() );
         } else {
            switch ( m_configDialog->onStartup() ) {
            case Marble::LastLocationVisited:
                m_controlView->marbleWidget()->centerOn(
                    settings.value("quitLongitude", 0.0).toDouble(),
                    settings.value("quitLatitude", 0.0).toDouble() );
                if (! isDistanceOverwritten) {
                    // set default radius to 1350 (Atlas theme's "sharp" radius)
                    m_controlView->marbleWidget()->setRadius( settings.value("quitRadius", 1350).toInt() );
                }
                break;
            case Marble::ShowHomeLocation:
                m_controlView->marbleWidget()->goHome();
                break;
            default:
                break;
            }
         }
         if (isDistanceOverwritten) {
             m_controlView->marbleWidget()->setDistance(distanceIt.value().toDouble());
         }

         // Geo URI parsing
         QString geoUriString = qvariant_cast<QString>( overrideSettings.value("geo-uri", ""));
         if ( !geoUriString.isEmpty() ) {
             m_controlView->openGeoUri( geoUriString );
         }

         bool isLocked = settings.value( "lockFloatItemPositions", false ).toBool();
         m_lockFloatItemsAction->setChecked( isLocked );
         lockPosition(isLocked);
     settings.endGroup();

     settings.beginGroup( "Sun" );
         m_controlView->marbleWidget()->setShowSunShading( settings.value( "showSun", false ).toBool() );
         m_controlView->marbleWidget()->setShowCityLights( settings.value( "showCitylights", false ).toBool() );
         m_controlView->marbleWidget()->setLockToSubSolarPoint( settings.value( "lockToSubSolarPoint", false ).toBool() );
         m_controlView->marbleWidget()->setSubSolarPointIconVisible( settings.value( "subSolarPointIconVisible", false ).toBool() );
     settings.endGroup();

     settings.beginGroup( "Time" );
        if( settings.value( "systemTime", "true" ).toBool() == true  )
        {
            /* nothing to do */
        }
        else if( settings.value( "lastSessionTime", "true" ).toBool() == true )
        {
            m_controlView->marbleModel()->setClockDateTime( settings.value( "dateTime" ).toDateTime() );
            m_controlView->marbleModel()->setClockSpeed( settings.value( "speedSlider", 1 ).toInt() );
        }
     settings.endGroup();

     setUpdatesEnabled(true);

     // Load previous route settings
     settings.beginGroup( "Routing" );
     {
         RoutingManager *const routingManager = m_controlView->marbleModel()->routingManager();
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
                    if (key != QLatin1String("Enabled")) {
                        profile.pluginSettings()[ pluginName ].insert( key, settings.value( key ) );
                    }
                }
                settings.endGroup();
            }
            profiles << profile;
            settings.endGroup();
        }
        m_controlView->marbleModel()->routingManager()->profilesModel()->setProfiles( profiles );
    } else {
        m_controlView->marbleModel()->routingManager()->profilesModel()->loadDefaultProfiles();
    }
    int const profileIndex = settings.value( "currentIndex", 0 ).toInt();
    if ( profileIndex >= 0 && profileIndex < m_controlView->marbleModel()->routingManager()->profilesModel()->rowCount() ) {
        RoutingProfile profile = m_controlView->marbleModel()->routingManager()->profilesModel()->profiles().at( profileIndex );
        m_controlView->marbleModel()->routingManager()->routeRequest()->setRoutingProfile( profile );
    }

    settings.endGroup();

    settings.beginGroup( "Plugins");
    PositionTracking* tracking = m_controlView->marbleModel()->positionTracking();
    tracking->readSettings();
    QString positionProvider = settings.value( "activePositionTrackingPlugin", QString() ).toString();
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
    settings.endGroup();

    settings.beginGroup( "Tracking" );
    if ( settings.contains( "autoCenter" ) || settings.contains( "recenterMode" ) ) {
        CurrentLocationWidget* trackingWidget = m_controlView->currentLocationWidget();
        Q_ASSERT( trackingWidget );
        trackingWidget->setRecenterMode( settings.value( "recenterMode", 0 ).toInt() );
        trackingWidget->setAutoZoom( settings.value( "autoZoom", false ).toBool() );
        trackingWidget->setTrackVisible( settings.value( "trackVisible", true ).toBool() );
        trackingWidget->setLastOpenPath( settings.value( "lastTrackOpenPath", QDir::homePath() ).toString() );
        trackingWidget->setLastSavePath( settings.value( "lastTrackSavePath", QDir::homePath() ).toString() );
    }
    settings.endGroup();

     // The config dialog has to read settings.
     m_configDialog->readSettings();

     settings.beginGroup( "Navigation" );
     m_controlView->setExternalMapEditor( settings.value( "externalMapEditor", "" ).toString() );
     settings.endGroup();

     settings.beginGroup( "CloudSync" );
     CloudSyncManager* cloudSyncManager = m_controlView->cloudSyncManager();
     cloudSyncManager->setOwncloudCredentials( settings.value( "owncloudServer", "" ).toString(),
                                               settings.value( "owncloudUsername", "" ).toString(),
                                               settings.value( "owncloudPassword", "" ).toString() );
     cloudSyncManager->setSyncEnabled( settings.value( "enableSync", false ).toBool() );
     cloudSyncManager->routeSyncManager()->setRouteSyncEnabled( settings.value( "syncRoutes", true ).toBool() );
     cloudSyncManager->bookmarkSyncManager()->setBookmarkSyncEnabled( settings.value( "syncBookmarks", true ).toBool() );
     settings.endGroup();
}

void MainWindow::writeSettings()
{
     QSettings settings;

     settings.beginGroup( "MainWindow" );
         settings.setValue( "size", size() );
         settings.setValue( "pos", pos() );
         settings.setValue( "fullScreen", m_fullScreenAction->isChecked() );
         settings.setValue( "statusBar", m_statusBarAction->isChecked() );
         settings.setValue( "showZoomLevel", m_toggleTileLevelAction->isChecked() );
         settings.setValue( "showClouds", m_showCloudsAction->isChecked() );
         settings.setValue( "workOffline", m_workOfflineAction->isChecked() );
         settings.setValue( "showAtmosphere", m_controlView->marbleWidget()->showAtmosphere() );
         settings.setValue( "lastFileOpenDir", m_lastFileOpenPath );
         settings.setValue( "showBookmarks", m_toggleBookmarkDisplayAction->isChecked() );
         settings.setValue( "windowState", saveState() );
     settings.endGroup();

     settings.beginGroup( "MarbleWidget" );
         // Get the 'home' values from the widget and store them in the settings.
         qreal homeLon = 0;
         qreal homeLat = 0;
         int homeZoom = 0;
         m_controlView->marbleModel()->home( homeLon, homeLat, homeZoom );
         QString  mapTheme = m_controlView->marbleWidget()->mapThemeId();
         int      projection = (int)( m_controlView->marbleWidget()->projection() );

         settings.setValue( "homeLongitude", homeLon );
         settings.setValue( "homeLatitude",  homeLat );
         settings.setValue( "homeZoom",      homeZoom );

         settings.setValue( "mapTheme",   mapTheme );
         settings.setValue( "projection", projection );

         // Get the 'quit' values from the widget and store them in the settings.
         qreal  quitLon = m_controlView->marbleWidget()->centerLongitude();
         qreal  quitLat = m_controlView->marbleWidget()->centerLatitude();
         const int quitRadius = m_controlView->marbleWidget()->radius();

         settings.setValue( "quitLongitude", quitLon );
         settings.setValue( "quitLatitude", quitLat );
         settings.setValue( "quitRadius", quitRadius );

         settings.setValue( "lockFloatItemPositions", m_lockFloatItemsAction->isChecked() );
     settings.endGroup();

     settings.beginGroup( "Sun" );
         settings.setValue( "showSun", m_controlView->marbleWidget()->showSunShading() );
         settings.setValue( "showCitylights", m_controlView->marbleWidget()->showCityLights() );
         settings.setValue( "lockToSubSolarPoint", m_controlView->marbleWidget()->isLockedToSubSolarPoint() );
         settings.setValue( "subSolarPointIconVisible", m_controlView->marbleWidget()->isSubSolarPointIconVisible() );
     settings.endGroup();

      settings.beginGroup( "Time" );
         settings.setValue( "dateTime", m_controlView->marbleModel()->clockDateTime() );
         settings.setValue( "speedSlider", m_controlView->marbleModel()->clockSpeed() );
     settings.endGroup();

     settings.beginGroup( "Routing Profile" );
     QList<RoutingProfile>  profiles = m_controlView->marbleWidget()
                         ->model()->routingManager()->profilesModel()->profiles();
     settings.setValue( "Num", profiles.count() );
     for ( int i = 0; i < profiles.count(); ++i ) {
         settings.beginGroup( QString( "Profile %0" ).arg(i) );
         const RoutingProfile& profile = profiles.at( i );
         settings.setValue( "Name", profile.name() );
         foreach ( const QString& pluginName, settings.childGroups() ) {
             settings.beginGroup( pluginName );
             settings.remove(QString()); //remove all keys
             settings.endGroup();
         }
         foreach ( const QString &key, profile.pluginSettings().keys() ) {
             settings.beginGroup( key );
             settings.setValue( "Enabled", true );
             foreach ( const QString& settingKey, profile.pluginSettings()[ key ].keys() ) {
                 Q_ASSERT(settingKey != QLatin1String("Enabled"));
                 settings.setValue( settingKey, profile.pluginSettings()[ key ][ settingKey ] );
             }
             settings.endGroup();
         }
         settings.endGroup();
     }
     RoutingProfile const profile = m_controlView->marbleWidget()->model()->routingManager()->routeRequest()->routingProfile();
     settings.setValue( "currentIndex", profiles.indexOf( profile ) );
     settings.endGroup();

     settings.beginGroup( "Plugins");
     QString positionProvider;
     PositionTracking* tracking = m_controlView->marbleModel()->positionTracking();
     tracking->writeSettings();
     if ( tracking->positionProviderPlugin() ) {
         positionProvider = tracking->positionProviderPlugin()->nameId();
     }
     settings.setValue( "activePositionTrackingPlugin", positionProvider );
     settings.endGroup();

     settings.beginGroup( "Tracking" );
     CurrentLocationWidget* trackingWidget = m_controlView->currentLocationWidget();
     if ( trackingWidget ) {
         // Can be null due to lazy initialization
         settings.setValue( "recenterMode", trackingWidget->recenterMode() );
         settings.setValue( "autoZoom", trackingWidget->autoZoom() );
         settings.setValue( "trackVisible", trackingWidget->trackVisible() );
         settings.setValue( "lastTrackOpenPath", trackingWidget->lastOpenPath() );
         settings.setValue( "lastTrackSavePath", trackingWidget->lastSavePath() );
     }
     settings.endGroup();

     // The config dialog has to write settings.
     m_configDialog->writeSettings();

     // Store current route settings
     settings.beginGroup( "Routing" );
     {
         RoutingManager *const routingManager = m_controlView->marbleModel()->routingManager();
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

     settings.beginGroup( "Navigation");
     settings.setValue( "externalMapEditor", m_controlView->externalMapEditor() );
     settings.endGroup();
}

void MainWindow::editSettings()
{
    // Show the settings dialog.
    m_configDialog->show();
    m_configDialog->raise();
    m_configDialog->activateWindow();
}

void MainWindow::updateSettings()
{
    mDebug() << Q_FUNC_INFO << "Updating Settings ...";

    // FIXME: Font doesn't get updated instantly.
    m_controlView->marbleWidget()->setDefaultFont( m_configDialog->mapFont() );

    m_controlView->marbleWidget()->setMapQualityForViewContext( m_configDialog->stillQuality(), Marble::Still );
    m_controlView->marbleWidget()->setMapQualityForViewContext( m_configDialog->animationQuality(), Marble::Animation );

    m_controlView->marbleWidget()->setDefaultAngleUnit( m_configDialog->angleUnit() );
    MarbleGlobal::getInstance()->locale()->setMeasurementSystem( m_configDialog->measurementSystem() );

    m_distance = m_controlView->marbleWidget()->distanceString();
    updateStatusBar();

    m_controlView->marbleWidget()->setAnimationsEnabled( m_configDialog->animateTargetVoyage() );
    m_controlView->marbleWidget()->inputHandler()->setInertialEarthRotationEnabled( m_configDialog->inertialEarthRotation() );
    if ( !m_configDialog->externalMapEditor().isEmpty() ) {
        m_controlView->setExternalMapEditor( m_configDialog->externalMapEditor() );
    }

    // Cache
    m_controlView->marbleModel()->setPersistentTileCacheLimit( m_configDialog->persistentTileCacheLimit() * 1024 );
    m_controlView->marbleWidget()->setVolatileTileCacheLimit( m_configDialog->volatileTileCacheLimit() * 1024 );

    /*
    m_controlView->marbleWidget()->setProxy( m_configDialog->proxyUrl(), m_configDialog->proxyPort(), m_configDialog->user(), m_configDialog->password() );
    */

    CloudSyncManager* cloudSyncManager = m_controlView->cloudSyncManager();
    cloudSyncManager->setOwncloudCredentials( m_configDialog->owncloudServer(),
                                              m_configDialog->owncloudUsername(),
                                              m_configDialog->owncloudPassword() );
    cloudSyncManager->setSyncEnabled( m_configDialog->syncEnabled() );
    cloudSyncManager->routeSyncManager()->setRouteSyncEnabled( m_configDialog->syncRoutes() );
    cloudSyncManager->bookmarkSyncManager()->setBookmarkSyncEnabled( m_configDialog->syncBookmarks() );

    m_controlView->marbleWidget()->update();
}

void MainWindow::showDownloadRegionDialog()
{
    if ( !m_downloadRegionDialog ) {
        m_downloadRegionDialog = new DownloadRegionDialog( m_controlView->marbleWidget(), m_controlView );
        // it might be tempting to move the connects to DownloadRegionDialog's "accepted" and
        // "applied" signals, be aware that the "hidden" signal might be come before the "accepted"
        // signal, leading to a too early disconnect.
        connect( m_downloadRegionDialog, SIGNAL(accepted()), SLOT(downloadRegion()));
        connect( m_downloadRegionDialog, SIGNAL(applied()), SLOT(downloadRegion()));
    }
    // FIXME: get allowed range from current map theme
    m_downloadRegionDialog->setAllowedTileLevelRange( 0, 16 );
    m_downloadRegionDialog->setSelectionMethod( DownloadRegionDialog::VisibleRegionMethod );
    ViewportParams const * const viewport =
        m_controlView->marbleWidget()->viewport();
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
        m_controlView->marbleWidget()->downloadRegion( pyramid );
    }
}

void MainWindow::printMapScreenShot()
{
#ifndef QT_NO_PRINTER
    QPrinter printer( QPrinter::HighResolution );
    QPointer<QPrintDialog> printDialog = new QPrintDialog( &printer, this );
    m_controlView->printMapScreenShot( printDialog );
    delete printDialog;
#endif
}

void MainWindow::updateMapEditButtonVisibility( const QString &mapTheme )
{
    Q_ASSERT( m_osmEditAction );
    QStringList osmThemes = QStringList()
            << "earth/openstreetmap/openstreetmap.dgml"
            << "earth/hikebikemap/hikebikemap.dgml"
            << "earth/opencyclemap/opencyclemap.dgml"
            << "earth/public-transport/public-transport.dgml"
            << "earth/openseamap/openseamap.dgml"
            << "earth/vectorosm/vectorosm.dgml";
    m_osmEditAction->setVisible(osmThemes.contains(mapTheme));
}

void MainWindow::showMovieCaptureDialog()
{
    if (m_movieCaptureDialog == 0) {
        m_movieCaptureDialog = new MovieCaptureDialog(m_controlView->marbleWidget(),
                                                      m_controlView->marbleWidget());
        connect( m_movieCaptureDialog, SIGNAL(started()), this, SLOT(changeRecordingState()));
    }
    m_movieCaptureDialog->show();
}

void MainWindow::stopRecording()
{
    if ( m_movieCaptureDialog ) {
        m_movieCaptureDialog->stopRecording();
        changeRecordingState();
    }
}

void MainWindow::changeRecordingState()
{
    m_recordMovieAction->setEnabled( !m_recordMovieAction->isEnabled() );
    m_stopRecordingAction->setEnabled( !m_stopRecordingAction->isEnabled() );
}

void MainWindow::updateWindowTitle()
{
    GeoSceneDocument *theme = m_controlView->marbleModel()->mapTheme();
    setWindowTitle(theme ? theme->head()->name() : QString());
}

void MainWindow::showMapWizard()
{
    QPointer<MapWizard> mapWizard = new MapWizard();
    QSettings settings;

    settings.beginGroup( "MapWizard" );
        mapWizard->setWmsServers( settings.value( "wmsServers" ).toStringList() );
        mapWizard->setStaticUrlServers( settings.value( "staticUrlServers" ).toStringList() );
    settings.endGroup();

    mapWizard->exec();

    settings.beginGroup( "MapWizard" );
        settings.setValue( "wmsServers", mapWizard->wmsServers() );
        settings.setValue( "staticUrlServers", mapWizard->staticUrlServers() );
    settings.endGroup();

    mapWizard->deleteLater();
}

void MainWindow::showZoomLevel(bool show)
{
    if ( show ) {
        m_zoomLabel->show();
        statusBar()->insertPermanentWidget( 2, m_zoomLabel );
    } else {
        statusBar()->removeWidget( m_zoomLabel );
    }
    // update from last modification
    m_toggleTileLevelAction->setChecked( show );
}

void MainWindow::changeAngleDisplayUnit( QAction *action )
{
    m_configDialog->setAngleUnit((Marble::AngleUnit)action->data().toInt());
}

void MainWindow::fallBackToDefaultTheme()
{
    m_controlView->marbleWidget()->setMapThemeId( m_controlView->defaultMapThemeId() );
}

void MainWindow::changeViewSize( QAction* action )
{
    if ( action->data().type() == QVariant::Size ) {
        if ( m_savedSize.isEmpty() ) {
            m_savedSize = m_controlView->size();
        }
        m_controlView->setFixedSize( action->data().toSize() );
        adjustSize();
    } else {
        m_controlView->setMinimumSize( QSize( 0, 0 ) );
        m_controlView->setMaximumSize( QSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX ) );
        m_controlView->resize( m_savedSize );
        m_controlView->setMinimumSize( m_savedSize );
        adjustSize();
        m_controlView->setMinimumSize( QSize( 0, 0 ) );
        m_savedSize.setHeight( -1 );
    }
}

#include "moc_QtMainWindow.cpp"
