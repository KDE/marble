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
#include <QTimer>
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
#include <QHBoxLayout>
#include <QScrollArea>
#include <QClipboard>
#include <QShortcut>
#include <QDockWidget>
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
#include "AbstractDataPlugin.h"
#include "AbstractFloatItem.h"
#include "MarbleModel.h"
#include "MarbleClock.h"
#include "HttpDownloadManager.h"
#include "BookmarkManager.h"
#include "NewBookmarkFolderDialog.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
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
#include "GoToDialog.h"
#include "MarbleWidgetInputHandler.h"
#include "Planet.h"
#include "cloudsync/CloudSyncManager.h"
#include "cloudsync/BookmarkSyncManager.h"

namespace
{
    const char* POSITION_STRING = "Position:";
    const char* DISTANCE_STRING = "Altitude:";
    const char* ZOOM_STRING = "Zoom:";
    const char* DATETIME_STRING = "Time:";
}

using namespace Marble;
/* TRANSLATOR Marble::MainWindow */

MainWindow::MainWindow(const QString& marbleDataPath, const QVariantMap& cmdLineSettings, QWidget *parent) :
        QMainWindow(parent),
        m_sunControlDialog( 0 ),
        m_timeControlDialog( 0 ),
        m_downloadRegionDialog( 0 ),
        m_panelMenu( 0 ),
        m_downloadRegionAction( 0 ),
        m_osmEditAction( 0 ),
        m_zoomLabel( 0 )
{
    setUpdatesEnabled( false );

    QString selectedPath = marbleDataPath.isEmpty() ? readMarbleDataPath() : marbleDataPath;
    if ( !selectedPath.isEmpty() )
        MarbleDirs::setMarbleDataPath( selectedPath );

    m_controlView = new ControlView( this );

    setWindowTitle( tr("Marble - Virtual Globe") );
    setWindowIcon( QIcon(":/icons/marble.png") );
    setCentralWidget( m_controlView );

    // Initializing config dialog
    m_configDialog = new QtMarbleConfigDialog( m_controlView->marbleWidget(), this );
    connect( m_configDialog, SIGNAL(settingsChanged()),
             this, SLOT(updateSettings()) );
    connect( m_configDialog, SIGNAL(clearVolatileCacheClicked()),
             m_controlView->marbleWidget(), SLOT(clearVolatileTileCache()) );
    connect( m_configDialog, SIGNAL(clearPersistentCacheClicked()),
             m_controlView->marbleModel(), SLOT(clearPersistentTileCache()) );
    connect( m_configDialog, SIGNAL(syncNowClicked()),
             m_controlView->cloudSyncManager()->bookmarkSyncManager(), SLOT(startBookmarkSync()) );

    // Load bookmark file. If it does not exist, a default one will be used.
    m_controlView->marbleModel()->bookmarkManager()->loadFile( "bookmarks/bookmarks.kml" );

    createActions();
    QList<QAction*> const panelActions = m_controlView->setupDockWidgets( this );
    createMenus( panelActions );
    createStatusBar();

    connect( m_controlView->marbleWidget(), SIGNAL(themeChanged(QString)),
             this, SLOT(updateMapEditButtonVisibility(QString)) );
    connect( m_controlView, SIGNAL(showMapWizard()), this, SLOT(showMapWizard()) );
    connect( m_controlView, SIGNAL(mapThemeDeleted()), this, SLOT(fallBackToDefaultTheme()) );

    setUpdatesEnabled( true );

    m_position = tr( NOT_AVAILABLE );
    m_distance = marbleWidget()->distanceString();
    m_zoom = QString::number( marbleWidget()->tileZoomLevel() );

    m_clock = QLocale().toString( m_controlView->marbleModel()->clockDateTime().addSecs( m_controlView->marbleModel()->clockTimezone() ), QLocale::ShortFormat );
    QMetaObject::invokeMethod(this,
                              "initObject", Qt::QueuedConnection,
                              Q_ARG(QVariantMap, cmdLineSettings));
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
    m_commandlineFilePaths.clear();
}

void MainWindow::createActions()
 {
     m_openAct = new QAction( QIcon(":/icons/document-open.png"), tr( "&Open..."), this );
     m_openAct->setShortcut( tr( "Ctrl+O" ) );
     m_openAct->setStatusTip( tr( "Open a file for viewing on Marble"));
     connect( m_openAct, SIGNAL(triggered()),
              this, SLOT(openFile()) );

     m_downloadAct = new QAction( QIcon(":/icons/get-hot-new-stuff.png"), tr("&Download Maps..."), this);
     connect(m_downloadAct, SIGNAL(triggered()), this, SLOT(openMapDialog()));

     m_exportMapAct = new QAction( QIcon(":/icons/document-save-as.png"), tr("&Export Map..."), this);
     m_exportMapAct->setShortcut(tr("Ctrl+S"));
     m_exportMapAct->setStatusTip(tr("Save a screenshot of the map"));
     connect(m_exportMapAct, SIGNAL(triggered()), this, SLOT(exportMapScreenShot()));

     // Action: Download Region
     m_downloadRegionAction = new QAction( tr( "Download &Region..." ), this );
     m_downloadRegionAction->setStatusTip( tr( "Download a map region in different zoom levels for offline usage" ) );
     connect( m_downloadRegionAction, SIGNAL(triggered()), SLOT(showDownloadRegionDialog()) );

     m_printAct = new QAction( QIcon(":/icons/document-print.png"), tr("&Print..."), this);
     m_printAct->setShortcut(tr("Ctrl+P"));
     m_printAct->setStatusTip(tr("Print a screenshot of the map"));
     connect(m_printAct, SIGNAL(triggered()), this, SLOT(printMapScreenShot()));

     m_printPreviewAct = new QAction( QIcon(":/icons/document-print-preview.png"), tr("Print Previe&w ..."), this);
     m_printPreviewAct->setStatusTip(tr("Print a screenshot of the map"));
     connect(m_printPreviewAct, SIGNAL(triggered()), m_controlView, SLOT(printPreview()));

     m_quitAct = new QAction( QIcon(":/icons/application-exit.png"), tr("&Quit"), this);
     m_quitAct->setShortcut(tr("Ctrl+Q"));
     m_quitAct->setStatusTip(tr("Quit the Application"));
     connect(m_quitAct, SIGNAL(triggered()), this, SLOT(close()));

     m_copyMapAct = new QAction( QIcon(":/icons/edit-copy.png"), tr("&Copy Map"), this);
     m_copyMapAct->setShortcut(tr("Ctrl+C"));
     m_copyMapAct->setStatusTip(tr("Copy a screenshot of the map"));
     connect(m_copyMapAct, SIGNAL(triggered()), this, SLOT(copyMap()));

     m_osmEditAction = new QAction( QIcon(":/icons/edit-map.png"), tr( "&Edit Map" ), this );
     m_osmEditAction->setShortcut(tr( "Ctrl+E" ) );
     m_osmEditAction->setStatusTip(tr( "Edit the current map region in an external editor" ) );
     updateMapEditButtonVisibility( m_controlView->marbleWidget()->mapThemeId() );
     connect( m_osmEditAction, SIGNAL(triggered()), m_controlView, SLOT(launchExternalMapEditor()) );

     m_configDialogAct = new QAction( QIcon(":/icons/settings-configure.png"),tr("&Configure Marble"), this);
     m_configDialogAct->setStatusTip(tr("Show the configuration dialog"));
     connect(m_configDialogAct, SIGNAL(triggered()), this, SLOT(editSettings()));

     m_copyCoordinatesAct = new QAction( QIcon(":/icons/copy-coordinates.png"), tr("C&opy Coordinates"), this);
     m_copyCoordinatesAct->setStatusTip(tr("Copy the center coordinates as text"));
     connect(m_copyCoordinatesAct, SIGNAL(triggered()), this, SLOT(copyCoordinates()));

     m_fullScreenAct = new QAction( QIcon(":/icons/view-fullscreen.png"), tr("&Full Screen Mode"), this);
     m_fullScreenAct->setShortcut(tr("Ctrl+Shift+F"));
     m_fullScreenAct->setCheckable( true );
     m_fullScreenAct->setStatusTip(tr("Full Screen Mode"));
     connect(m_fullScreenAct, SIGNAL(triggered(bool)), this, SLOT(showFullScreen(bool)));

     m_statusBarAct = new QAction( tr("&Show Status Bar"), this);
     m_statusBarAct->setCheckable( true );
     m_statusBarAct->setStatusTip(tr("Show Status Bar"));
     connect(m_statusBarAct, SIGNAL(triggered(bool)), this, SLOT(showStatusBar(bool)));


     m_lockFloatItemsAct = new QAction( QIcon(":/icons/unlock.png"), tr("Lock Position"),this);
     m_lockFloatItemsAct->setCheckable( true );
     m_lockFloatItemsAct->setStatusTip(tr("Lock Position of Floating Items"));
     connect(m_lockFloatItemsAct, SIGNAL(triggered(bool)), this, SLOT(lockPosition(bool)));

     m_showCloudsAct = new QAction( QIcon(":/icons/clouds.png"), tr("&Clouds"), this);
     m_showCloudsAct->setCheckable( true );
     m_showCloudsAct->setStatusTip(tr("Show Real Time Cloud Cover"));
     connect(m_showCloudsAct, SIGNAL(triggered(bool)), this, SLOT(showClouds(bool)));

     m_workOfflineAct = new QAction( QIcon(":/icons/user-offline.png"), tr("Work Off&line"), this);
     m_workOfflineAct->setCheckable( true );
     connect(m_workOfflineAct, SIGNAL(triggered(bool)), this, SLOT(workOffline(bool)));

     m_controlTimeAct = new QAction( QIcon(":/icons/clock.png"), tr( "&Time Control..." ), this );
     m_controlTimeAct->setStatusTip( tr( "Configure Time Control " ) );
     connect( m_controlTimeAct, SIGNAL(triggered()), this, SLOT(controlTime()) );

     m_controlSunAct = new QAction( tr( "S&un Control..." ), this );
     m_controlSunAct->setStatusTip( tr( "Configure Sun Control" ) );
     connect( m_controlSunAct, SIGNAL(triggered()), this, SLOT(controlSun()) );

     m_reloadAct = new QAction( QIcon(":/icons/view-refresh.png"), tr("&Redisplay"), this);
     m_reloadAct->setShortcut(tr("F5"));
     m_reloadAct->setStatusTip(tr("Reload Current Map"));
     connect(m_reloadAct, SIGNAL(triggered()), this, SLOT(reload()));

     m_handbookAct = new QAction( QIcon(":/icons/help-contents.png"), tr("Marble Virtual Globe &Handbook"), this);
     m_handbookAct->setShortcut(tr("F1"));
     m_handbookAct->setStatusTip(tr("Show the Handbook for Marble Virtual Globe"));
     connect(m_handbookAct, SIGNAL(triggered()), this, SLOT(handbook()));

     m_whatsThisAct = new QAction( QIcon(":/icons/help-whatsthis.png"), tr("What's &This"), this);
     m_whatsThisAct->setShortcut(tr("Shift+F1"));
     m_whatsThisAct->setStatusTip(tr("Show a detailed explanation of the action."));
     connect(m_whatsThisAct, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

     m_aboutMarbleAct = new QAction( QIcon(":/icons/marble.png"), tr("&About Marble Virtual Globe"), this);
     m_aboutMarbleAct->setStatusTip(tr("Show the application's About Box"));
     connect(m_aboutMarbleAct, SIGNAL(triggered()), this, SLOT(aboutMarble()));

     m_aboutQtAct = new QAction(tr("About &Qt"), this);
     m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
     connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

     //Bookmark Actions
     m_addBookmarkAct = new QAction( QIcon(":/icons/bookmark-new.png"), tr("&Add Bookmark"),this);
     m_addBookmarkAct->setShortcut(tr("Ctrl+B"));
     m_addBookmarkAct->setStatusTip(tr("Add Bookmark"));
     connect( m_addBookmarkAct, SIGNAL(triggered()), this, SLOT(openEditBookmarkDialog()) );

     m_setHomeAct = new QAction( QIcon(":/icons/go-home.png"), tr( "&Set Home Location" ),this);
     m_setHomeAct->setStatusTip( tr( "&Set Home Location" ) );
     connect( m_setHomeAct, SIGNAL(triggered()), this, SLOT(setHome()) );

     m_toggleBookmarkDisplayAct = new QAction(tr( "Show &Bookmarks" ), this);
     m_toggleBookmarkDisplayAct->setStatusTip( tr( "Toggle display of Bookmarks" ) );
     m_toggleBookmarkDisplayAct->setCheckable( true );
     connect( m_toggleBookmarkDisplayAct, SIGNAL(triggered(bool)), this, SLOT(showBookmarks(bool)) );

     m_manageBookmarksAct = new QAction( QIcon( ":/icons/bookmarks-organize.png" ), tr( "&Manage Bookmarks" ), this);
     m_manageBookmarksAct->setStatusTip( tr( "Manage Bookmarks" ) );
     connect( m_manageBookmarksAct, SIGNAL(triggered()), this, SLOT(manageBookmarks()) );
     
     // Map Wizard action
     m_mapWizardAct = new QAction( QIcon( ":/icons/create-new-map.png" ), tr("&Create a New Map..."), this );
     m_mapWizardAct->setStatusTip( tr( "A wizard guides you through the creation of your own map theme." ) );
     connect( m_mapWizardAct, SIGNAL(triggered()), SLOT(showMapWizard()) );
}

void MainWindow::createMenus( const QList<QAction*> &panelActions )
{
        m_fileMenu = menuBar()->addMenu(tr("&File"));
        m_fileMenu->addAction(m_openAct);
        m_fileMenu->addAction(m_downloadAct);
        m_fileMenu->addAction( m_downloadRegionAction );
        m_fileMenu->addAction( m_mapWizardAct );
        m_fileMenu->addAction(m_exportMapAct);
        m_fileMenu->addSeparator();
        m_fileMenu->addAction(m_printAct);
        m_fileMenu->addAction(m_printPreviewAct);
        m_fileMenu->addSeparator();
        m_fileMenu->addAction(m_workOfflineAct);
        m_fileMenu->addAction(m_quitAct);

        m_fileMenu = menuBar()->addMenu(tr("&Edit"));
        m_fileMenu->addAction(m_copyMapAct);
        m_fileMenu->addAction(m_copyCoordinatesAct);
        m_fileMenu->addAction( m_osmEditAction );

        m_viewMenu = menuBar()->addMenu(tr("&View"));
        m_infoBoxesMenu = new QMenu( "&Info Boxes" );
        m_onlineServicesMenu = new QMenu( "&Online Services" );
        createPluginsMenus();

        m_bookmarkMenu = menuBar()->addMenu(tr("&Bookmarks"));
        createBookmarkMenu();
        connect( m_bookmarkMenu, SIGNAL(aboutToShow()), this, SLOT(createBookmarkMenu()) );

        m_panelMenu = new QMenu( "&Panels" );
        foreach( QAction* action, panelActions ) {
            m_panelMenu->addAction( action );
        }

        m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
        m_settingsMenu->addMenu( m_panelMenu );
        m_settingsMenu->addAction(m_statusBarAct);
        m_settingsMenu->addAction(m_fullScreenAct);
        m_settingsMenu->addSeparator();
        m_settingsMenu->addAction(m_configDialogAct);

        m_helpMenu = menuBar()->addMenu(tr("&Help"));
        m_helpMenu->addAction(m_handbookAct);
        m_helpMenu->addSeparator();
        m_helpMenu->addAction(m_whatsThisAct);
        m_helpMenu->addSeparator();
        m_helpMenu->addAction(m_aboutMarbleAct);
        m_helpMenu->addAction(m_aboutQtAct);

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

    m_viewMenu->addAction(m_reloadAct);
    m_viewMenu->addSeparator();

    // Do not create too many menu entries on a MID
    // FIXME: Set up another way of switching the plugins on and off.
    if( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        return;
    }

    m_infoBoxesMenu->addAction(m_lockFloatItemsAct);
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
            break;
        }
    }
    m_viewMenu->addMenu( m_infoBoxesMenu );
    m_viewMenu->addMenu( m_onlineServicesMenu );
    m_viewMenu->addActions( themeActions );
    m_viewMenu->addAction( m_showCloudsAct );
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_controlSunAct);
    m_viewMenu->addAction(m_controlTimeAct);
}

void MainWindow::createBookmarksListMenu( QMenu *bookmarksListMenu, const GeoDataContainer *container )
{
    //m_bookmarksListMenu->clear();

    QVector<GeoDataPlacemark*> bookmarks = container->placemarkList();

    foreach ( const GeoDataPlacemark *placemark, bookmarks ) {
        QAction *bookmarkAct = new QAction( placemark->name(), this );
        QVariant var;

        const GeoDataLookAt* lookAt = placemark->lookAt();
        if ( !lookAt ) {
            GeoDataCoordinates coordinates = placemark->coordinate( m_controlView->marbleModel()->clockDateTime() );
            GeoDataLookAt coordinateToLookAt;
            coordinateToLookAt.setCoordinates( coordinates );
            coordinateToLookAt.setRange( marbleWidget()->lookAt().range() );
            var.setValue( coordinateToLookAt );
        } else {
            var.setValue( *lookAt );
        }
        bookmarkAct->setData( var );
        bookmarksListMenu->addAction( bookmarkAct );
    }

}
void MainWindow::createBookmarkMenu()
{
    m_bookmarkMenu->clear();
    m_bookmarkMenu->addAction( m_addBookmarkAct );
    m_bookmarkMenu->addAction( m_toggleBookmarkDisplayAct );
    m_toggleBookmarkDisplayAct->setChecked( m_controlView->marbleModel()->bookmarkManager()->document()->isVisible() );
    m_bookmarkMenu->addAction( m_setHomeAct );
    m_bookmarkMenu->addAction( m_manageBookmarksAct );

    m_bookmarkMenu->addSeparator();

    m_bookmarkMenu->addAction( QIcon( ":/icons/go-home.png" ), "&Home",
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
            QMenu *subMenu = bookmarksListMenu->addMenu( QIcon( ":/icons/folder-bookmark.png" ), folder->name() );
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
#ifdef Q_WS_MAEMO_5
    dialog->setButtonBoxVisible( false );
    dialog->setAttribute( Qt::WA_Maemo5StackedWindow );
    dialog->setWindowFlags( Qt::Window );
#endif // Q_WS_MAEMO_5
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
        delete tb;
    }

    // Do not create too many menu entries on a MID
    // FIXME: Set up another way of switching the plugins on and off.
    if( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        return;
    }

    //remove and delete old menus if they exist
    while(  !m_pluginMenus.isEmpty() ) {
        m_pluginMenus.takeFirst();
//        FIXME: this does not provide an easy way to remove a menu.
//        Make a workaround
//        this->menuBar()->removeAction();
    }

    const QList<QActionGroup*> *tmp_toolbarActionGroups;
    QList<RenderPlugin *> renderPluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = renderPluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = renderPluginList.constEnd();

    //Load the toolbars
    for (; i != end; ++i ) {
        tmp_toolbarActionGroups = (*i)->toolbarActionGroups();

        if ( tmp_toolbarActionGroups ) {
            QToolBar* toolbar = new QToolBar(this);
            toolbar->setObjectName( QString( "plugin-toolbar-%1" ).arg( (*i)->nameId() ) );

            foreach( QActionGroup* ag, *tmp_toolbarActionGroups ) {
                toolbar->addActions( ag->actions() );
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
           | !fileName.endsWith(QLatin1String( "jpg" ), Qt::CaseInsensitive) )
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

    m_fullScreenAct->setChecked( isChecked ); // Sync state with the GUI
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

    m_statusBarAct->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::showClouds( bool isChecked )
{
    m_controlView->marbleWidget()->setShowClouds( isChecked );

    m_showCloudsAct->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::showBookmarks( bool show )
{
    m_controlView->marbleModel()->bookmarkManager()->setShowBookmarks( show );

    m_toggleBookmarkDisplayAct->setChecked( show ); // Sync state with the GUI
}

void MainWindow::workOffline( bool offline )
{
    m_controlView->setWorkOffline( offline );

    m_workOfflineAct->setChecked( offline ); // Sync state with the GUI
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

    QUrl handbookLocation( "http://docs.kde.org/stable/" + code + "/kdeedu/marble/index.html" );

    if ( handbookLocation.isEmpty() )
        handbookLocation = QUrl("http://docs.kde.org/stable/en/kdeedu/marble/index.html");

    if( !QDesktopServices::openUrl( handbookLocation ) )
    qDebug() << "URL not opened";
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
        m_positionLabel->setText( QString( "%1 %2" ).
        arg( tr( POSITION_STRING ) ).arg( m_position ) );

    if ( m_distanceLabel )
        m_distanceLabel->setText( QString( "%1 %2" )
        .arg( tr( DISTANCE_STRING ) ).arg( m_distance ) );

    if ( m_zoomLabel )
        m_zoomLabel->setText( QString( "%1 %2" )
        .arg( tr( ZOOM_STRING ) ).arg( m_zoom ) );

    if ( m_clockLabel )
        m_clockLabel->setText( QString( "%1 %2" )
        .arg( tr( DATETIME_STRING ) ).arg( m_clock ) );
}

void MainWindow::openFile()
{
    const PluginManager *const pluginManager = m_controlView->marbleModel()->pluginManager();

    QStringList allFileExtensions;
    QStringList filters;
    foreach ( const ParseRunnerPlugin *plugin, pluginManager->parsingRunnerPlugins() ) {
        if ( plugin->nameId() == "Cache" )
            continue;

        const QStringList fileExtensions = plugin->fileExtensions().replaceInStrings( QRegExp( "^" ), "*." );
        const QString filter = QString( "%1 (%2)" ).arg( plugin->fileFormatDescription() ).arg( fileExtensions.join( " " ) );
        filters << filter;
        allFileExtensions << fileExtensions;
    }

    allFileExtensions.sort();  // sort since file extensions are visible under Windows
    const QString allFileTypes = QString( "%1 (%2)" ).arg( tr( "All Supported Files" ) ).arg( allFileExtensions.join( " " ) );

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

    QAction* toggleTileLevelAction = new QAction( "Show zoom level", statusBar() );
    toggleTileLevelAction->setCheckable( true );
    toggleTileLevelAction->setChecked( false );
    connect( toggleTileLevelAction, SIGNAL(triggered(bool)),
             this, SLOT(showZoomLevel(bool)) );
    statusBar()->addAction( toggleTileLevelAction );

    setupDownloadProgressBar();

    m_positionLabel = new QLabel( );
    m_positionLabel->setIndent( 5 );
    QString templatePositionString =
        QString( "%1 000\xb0 00\' 00\"_, 000\xb0 00\' 00\"_" ).arg(POSITION_STRING);
    int maxPositionWidth = fontMetrics().boundingRect(templatePositionString).width()
                            + 2 * m_positionLabel->margin() + 2 * m_positionLabel->indent();
    m_positionLabel->setFixedWidth( maxPositionWidth );
    statusBar()->addPermanentWidget ( m_positionLabel );

    m_distanceLabel = new QLabel( );
    m_distanceLabel->setIndent( 5 );
    QString templateDistanceString =
        QString( "%1 00.000,0 mu" ).arg(DISTANCE_STRING);
    int maxDistanceWidth = fontMetrics().boundingRect(templateDistanceString).width()
                            + 2 * m_distanceLabel->margin() + 2 * m_distanceLabel->indent();
    m_distanceLabel->setFixedWidth( maxDistanceWidth );
    statusBar()->addPermanentWidget ( m_distanceLabel );

    m_zoomLabel = new QLabel( );
    m_zoomLabel->setIndent( 5 );
    QString templateZoomString =
        QString( "%1 00" ).arg(ZOOM_STRING);
    int maxZoomWidth = fontMetrics().boundingRect(templateZoomString).width()
                            + 2 * m_zoomLabel->margin() + 2 * m_zoomLabel->indent();
    m_zoomLabel->setFixedWidth( maxZoomWidth );
    // Not added here, but activated by the user with the context menu

    m_clockLabel = new QLabel( );
    m_clockLabel->setIndent( 5 );
    QString templateDateTimeString = QString( "%1 %2" ).arg( DATETIME_STRING , QLocale().toString( QDateTime::fromString ( "01:01:1000", "dd:mm:yyyy"), QLocale::ShortFormat ) );
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
    connect( downloadManager, SIGNAL(jobAdded()), SLOT(downloadJobAdded()) );
    connect( downloadManager, SIGNAL(jobRemoved()), SLOT(downloadJobRemoved()) );
}

void MainWindow::downloadJobAdded(){
    m_downloadProgressBar->setUpdatesEnabled( false );
    if ( m_downloadProgressBar->value() < 0 ) {
        m_downloadProgressBar->setMaximum( 1 );
        m_downloadProgressBar->setValue( 0 );
        m_downloadProgressBar->setVisible( true );
    } else {
        m_downloadProgressBar->setMaximum( m_downloadProgressBar->maximum() + 1 );
    }

    m_downloadProgressBar->setUpdatesEnabled( true );
}

void MainWindow::downloadJobRemoved(){
    m_downloadProgressBar->setUpdatesEnabled( false );
    m_downloadProgressBar->setValue( m_downloadProgressBar->value() + 1 );
    if ( m_downloadProgressBar->value() == m_downloadProgressBar->maximum() ) {
        m_downloadProgressBar->reset();
        m_downloadProgressBar->setVisible( false );
    }

    m_downloadProgressBar->setUpdatesEnabled( true );
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
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

         bool isLocked = settings.value( "lockFloatItemPositions", false ).toBool();
         m_lockFloatItemsAct->setChecked( isLocked );
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
                    if ( key != "Enabled" ) {
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
                instance->setMarbleModel( m_controlView->marbleModel() );
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
     cloudSyncManager->setOwncloudServer( settings.value( "owncloudServer", "" ).toString() );
     cloudSyncManager->setOwncloudUsername( settings.value( "owncloudUsername", "" ).toString() );
     cloudSyncManager->setOwncloudPassword( settings.value( "owncloudPassword", "" ).toString() );
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
         settings.setValue( "fullScreen", m_fullScreenAct->isChecked() );
         settings.setValue( "statusBar", m_statusBarAct->isChecked() );
         settings.setValue( "showClouds", m_showCloudsAct->isChecked() );
         settings.setValue( "workOffline", m_workOfflineAct->isChecked() );
         settings.setValue( "showAtmosphere", m_controlView->marbleWidget()->showAtmosphere() );
         settings.setValue( "lastFileOpenDir", m_lastFileOpenPath );
         settings.setValue( "showBookmarks", m_toggleBookmarkDisplayAct->isChecked() );
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

         settings.setValue( "lockFloatItemPositions", m_lockFloatItemsAct->isChecked() );
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
     RoutingProfile const profile = m_controlView->marbleWidget()->model()->routingManager()->routeRequest()->routingProfile();
     settings.setValue( "currentIndex", profiles.indexOf( profile ) );
     settings.endGroup();

     settings.beginGroup( "Plugins");
     QString positionProvider;
     PositionTracking* tracking = m_controlView->marbleModel()->positionTracking();
     tracking->writeSettings();
     if ( tracking && tracking->positionProviderPlugin() ) {
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
    cloudSyncManager->setOwncloudServer( m_configDialog->owncloudServer() );
    cloudSyncManager->setOwncloudUsername( m_configDialog->owncloudUsername() );
    cloudSyncManager->setOwncloudPassword( m_configDialog->owncloudPassword() );
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
    m_osmEditAction->setVisible( mapTheme == "earth/openstreetmap/openstreetmap.dgml" );
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
}

void MainWindow::fallBackToDefaultTheme()
{
    m_controlView->marbleWidget()->setMapThemeId( m_controlView->defaultMapThemeId() );
}

#include "QtMainWindow.moc"
