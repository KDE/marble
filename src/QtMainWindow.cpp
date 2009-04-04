//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "QtMainWindow.h"

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QCloseEvent>

#include <QtGui/QAction>
#include <QtGui/QLabel>
#include <QtGui/QWhatsThis>
#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QDesktopServices>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <QtGui/QPrintDialog>
#include <QtGui/QPrinter>
#include <QtGui/QPainter>

#include <QtGui/QClipboard>

#include <MarbleDirs.h>
#include "lib/MarbleAboutDialog.h"
#include "lib/SunControlWidget.h"
#include "lib/MarbleLocale.h"

#include "AbstractFloatItem.h"

#include "QtMarbleConfigDialog.h"

namespace
{
    const char* POSITION_STRING = "Position:";
    const char* DISTANCE_STRING = "Altitude:";
}

using namespace Marble;

MainWindow::MainWindow(const QString& marbleDataPath, QWidget *parent) : QMainWindow(parent), m_sunControlDialog(0)
{
    setUpdatesEnabled( false );
    
    QString selectedPath = ( marbleDataPath.isEmpty() ) ? readMarbleDataPath() : marbleDataPath;
    if ( !selectedPath.isEmpty() )
        MarbleDirs::setMarbleDataPath( selectedPath );

    m_controlView = new ControlView( this );

    setWindowTitle( tr("Marble - Desktop Globe") );
    setWindowIcon( QIcon(":/icons/marble.png") );
    setCentralWidget( m_controlView );
    
    // Initializing config dialog
    m_configDialog = new QtMarbleConfigDialog( this );
    connect( m_configDialog, SIGNAL( settingsChanged() ),
	     this, SLOT( updateSettings() ) );
    connect( m_configDialog, SIGNAL( clearVolatileCacheClicked() ),
	     m_controlView->marbleWidget(), SLOT( clearVolatileTileCache() ) );
    connect( m_configDialog, SIGNAL( clearPersistentCacheClicked() ),
	     m_controlView->marbleWidget(), SLOT( clearPersistentTileCache() ) );

    createActions();
    createMenus();
    createStatusBar();

    setUpdatesEnabled( true );

    m_position = NOT_AVAILABLE;
    m_distance = marbleWidget()->distanceString();

    QTimer::singleShot( 0, this, SLOT( initObject() ) );
}

void MainWindow::initObject()
{
    QCoreApplication::processEvents ();
    setupStatusBar();
    readSettings();
}

void MainWindow::createActions()
 {
     m_openAct = new QAction( QIcon(":/icons/document-open.png"), tr( "&Open Map..."), this );
     m_openAct->setShortcut( tr( "Ctrl+O" ) );
     m_openAct->setStatusTip( tr( "Open a file for viewing on Marble"));
     connect( m_openAct, SIGNAL( triggered() ),
              this, SLOT( openFile() ) );

     m_exportMapAct = new QAction( QIcon(":/icons/document-save-as.png"), tr("&Export Map..."), this);
     m_exportMapAct->setShortcut(tr("Ctrl+S"));
     m_exportMapAct->setStatusTip(tr("Save a screenshot of the map"));
     connect(m_exportMapAct, SIGNAL(triggered()), this, SLOT(exportMapScreenShot()));

     m_printAct = new QAction( QIcon(":/icons/document-print.png"), tr("&Print..."), this);
     m_printAct->setShortcut(tr("Ctrl+P"));
     m_printAct->setStatusTip(tr("Print a screenshot of the map"));
     connect(m_printAct, SIGNAL(triggered()), this, SLOT(printMapScreenShot()));

     m_quitAct = new QAction( QIcon(":/icons/application-exit.png"), tr("&Quit"), this);
     m_quitAct->setShortcut(tr("Ctrl+Q"));
     m_quitAct->setStatusTip(tr("Quit the Application"));
     connect(m_quitAct, SIGNAL(triggered()), this, SLOT(close()));

     m_copyMapAct = new QAction( QIcon(":/icons/edit-copy.png"), tr("&Copy Map"), this);
     m_copyMapAct->setShortcut(tr("Ctrl+C"));
     m_copyMapAct->setStatusTip(tr("Copy a screenshot of the map"));
     connect(m_copyMapAct, SIGNAL(triggered()), this, SLOT(copyMap()));
     
     m_configDialogAct = new QAction( tr("&Settings"), this);
     m_configDialogAct->setStatusTip(tr("Show the configuration dialog"));
     connect(m_configDialogAct, SIGNAL(triggered()), this, SLOT(editSettings()));

     m_copyCoordinatesAct = new QAction( QIcon(":/icons/edit-copy.png"), tr("C&opy Coordinates"), this);
     m_copyCoordinatesAct->setStatusTip(tr("Copy the center coordinates as text"));
     connect(m_copyCoordinatesAct, SIGNAL(triggered()), this, SLOT(copyCoordinates()));

     m_sideBarAct = new QAction( tr("Show &Navigation Panel"), this);
     m_sideBarAct->setShortcut(tr("F9"));
     m_sideBarAct->setCheckable( true );
     m_sideBarAct->setChecked( true );
     m_sideBarAct->setStatusTip(tr("Show Navigation Panel"));
     connect(m_sideBarAct, SIGNAL(triggered( bool )), this, SLOT( showSideBar( bool )));

     m_fullScreenAct = new QAction( tr("&Full Screen Mode"), this);
     m_fullScreenAct->setShortcut(tr("Ctrl+Shift+F"));
     m_fullScreenAct->setCheckable( true );
     m_fullScreenAct->setStatusTip(tr("Full Screen Mode"));
     connect(m_fullScreenAct, SIGNAL(triggered( bool )), this, SLOT( showFullScreen( bool )));

     m_statusBarAct = new QAction( tr("&Status Bar"), this);
     m_statusBarAct->setCheckable( true );
     m_statusBarAct->setStatusTip(tr("Show Status Bar"));
     connect(m_statusBarAct, SIGNAL(triggered( bool )), this, SLOT( showStatusBar( bool )));


     m_lockFloatItemsAct = new QAction( tr("Lock Position"),this);
     m_lockFloatItemsAct->setCheckable( true );
     m_lockFloatItemsAct->setStatusTip(tr("Lock Position of Floating Items"));
     connect(m_lockFloatItemsAct, SIGNAL(triggered( bool )), this, SLOT( lockPosition( bool )));

     m_showCloudsAct = new QAction( tr("&Clouds"), this);
     m_showCloudsAct->setCheckable( true );
     m_showCloudsAct->setStatusTip(tr("Show Real Time Cloud Cover"));
     connect(m_showCloudsAct, SIGNAL(triggered( bool )), this, SLOT( showClouds( bool )));

     m_workOfflineAct = new QAction( QIcon(":/icons/user-offline.png"), tr("&Work Offline"), this);
     m_workOfflineAct->setCheckable( true );
     connect(m_workOfflineAct, SIGNAL(triggered( bool )), this, SLOT( workOffline( bool )));

     m_showAtmosphereAct = new QAction( tr("&Atmosphere"), this);
     m_showAtmosphereAct->setCheckable( true );
     m_showAtmosphereAct->setStatusTip(tr("Show Atmosphere"));
     connect(m_showAtmosphereAct, SIGNAL(triggered( bool )), this, SLOT( showAtmosphere( bool )));

     m_controlSunAct = new QAction( tr("S&un Control..."), this);
     m_controlSunAct->setStatusTip(tr("Configure Sun Control"));
     connect(m_controlSunAct, SIGNAL(triggered()), this, SLOT( controlSun()));

     m_handbook = new QAction( QIcon(":/icons/help-contents.png"), tr("Marble Desktop Globe &Handbook"), this);
     m_handbook->setShortcut(tr("F1"));
     m_handbook->setStatusTip(tr("Show the Handbook for Marble Desktop Globe"));
     connect(m_handbook, SIGNAL(triggered()), this, SLOT(handbook()));

     m_whatsThisAct = new QAction( QIcon(":/icons/help-whatsthis.png"), tr("What's &This"), this);
     m_whatsThisAct->setShortcut(tr("Shift+F1"));
     m_whatsThisAct->setStatusTip(tr("Show a detailed explanation of the action."));
     connect(m_whatsThisAct, SIGNAL(triggered()), this, SLOT(enterWhatsThis()));

     m_aboutMarbleAct = new QAction( QIcon(":/icons/marble.png"), tr("&About Marble Desktop Globe"), this);
     m_aboutMarbleAct->setStatusTip(tr("Show the application's About Box"));
     connect(m_aboutMarbleAct, SIGNAL(triggered()), this, SLOT(aboutMarble()));

     m_aboutQtAct = new QAction(tr("About &Qt"), this);
     m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
     connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

}

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_exportMapAct);
    m_fileMenu->addAction(m_printAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_workOfflineAct);
    m_fileMenu->addAction(m_quitAct);

    m_fileMenu = menuBar()->addMenu(tr("&Edit"));
    m_fileMenu->addAction(m_copyMapAct);
    m_fileMenu->addAction(m_copyCoordinatesAct);

    m_fileMenu = menuBar()->addMenu(tr("&View"));

    QList<RenderPlugin *> pluginList = m_controlView->marbleWidget()->renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    for (; i != pluginList.constEnd(); ++i) {
        if ( (*i)->nameId() == "crosshairs" ) {
            m_fileMenu->addAction( (*i)->action() );
        }
    }

    m_fileMenu->addSeparator();
    m_infoBoxesMenu = m_fileMenu->addMenu("&Info Boxes");
    createInfoBoxesMenu();

    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_showCloudsAct);
    m_fileMenu->addAction(m_showAtmosphereAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_controlSunAct);

    m_fileMenu = menuBar()->addMenu(tr("&Settings"));
    m_fileMenu->addAction(m_statusBarAct);
    m_fileMenu->addAction(m_sideBarAct);
    m_fileMenu->addAction(m_fullScreenAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_configDialogAct);

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_handbook);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_whatsThisAct);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_aboutMarbleAct);
    m_helpMenu->addAction(m_aboutQtAct);
    

    connect( m_infoBoxesMenu, SIGNAL( aboutToShow() ), this, SLOT( createInfoBoxesMenu() ) ); 
}

void MainWindow::createInfoBoxesMenu()
{
    m_infoBoxesMenu->clear();
    m_infoBoxesMenu->addAction(m_lockFloatItemsAct);
    m_infoBoxesMenu->addSeparator();

    QList<AbstractFloatItem *> floatItemList = m_controlView->marbleWidget()->floatItems();

    QList<AbstractFloatItem *>::const_iterator i = floatItemList.constBegin();
    for (; i != floatItemList.constEnd(); ++i)
    {
        m_infoBoxesMenu->addAction( (*i)->action() );
    }
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
    statusBar()->hide();
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
        if ( !fileName.endsWith("png", Qt::CaseInsensitive) 
           | !fileName.endsWith("jpg", Qt::CaseInsensitive) )
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

void MainWindow::printMapScreenShot()
{
    QPixmap mapPixmap = m_controlView->mapScreenShot();

    QSize printSize = mapPixmap.size();

    QPrinter printer;

    QPrintDialog printDialog( &printer, this );

    if (printDialog.exec() == QDialog::Accepted) {

        QRect mapPageRect = printer.pageRect();

        printSize.scale( ( printer.pageRect() ).size(), Qt::KeepAspectRatio );

        QPoint printTopLeft( mapPageRect.x() + mapPageRect.width() /2  - printSize.width() /2 ,
                             mapPageRect.y() + mapPageRect.height()/2  - printSize.height()/2 );

        QRect mapPrintRect( printTopLeft, printSize );

        QPainter painter( &printer );

        painter.drawPixmap( mapPrintRect, mapPixmap, mapPixmap.rect() );

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

void MainWindow::showSideBar( bool isChecked )
{
    m_controlView->setSideBarShown( isChecked );

    m_sideBarAct->setChecked( isChecked ); // Sync state with the GUI
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

void MainWindow::workOffline( bool offline )
{
    if ( offline ) {
        m_controlView->marbleWidget()->setDownloadManager( 0 );
    }
    else {
        m_controlView->marbleWidget()->setDownloadUrl( "http://download.kde.org/apps/marble/" );
    }
}

void MainWindow::showAtmosphere( bool isChecked )
{
    m_controlView->marbleWidget()->setShowAtmosphere( isChecked );

    m_showAtmosphereAct->setChecked( isChecked ); // Sync state with the GUI
}

void MainWindow::lockPosition( bool isChecked )
{
    QList<AbstractFloatItem *> floatItemList = m_controlView->marbleWidget()->floatItems();

    QList<AbstractFloatItem *>::const_iterator i;
    for (i = floatItemList.constBegin(); i != floatItemList.constEnd(); ++i) 
    {
        // Locking one would suffice as it affects all. 
	// Nevertheless go through all.
        (*i)->setPositionLocked(isChecked);
    }
}

void MainWindow::controlSun()
{
    if (!m_sunControlDialog) {
        m_sunControlDialog = new SunControlWidget( this, m_controlView->sunLocator() );
        connect( m_sunControlDialog, SIGNAL( showSun( bool ) ),
                 this,               SLOT ( showSun( bool ) ) );
    }

     m_sunControlDialog->show();
     m_sunControlDialog->raise();
     m_sunControlDialog->activateWindow();
}

void MainWindow::showSun( bool active )
{
    m_controlView->marbleWidget()->sunLocator()->setShow( active ); 
}

void MainWindow::enterWhatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

void MainWindow::aboutMarble()
{
    MarbleAboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::handbook()
{
    if( !QDesktopServices::openUrl( QUrl( "http://docs.kde.org/stable/en/kdeedu/marble/index.html" ) ) ) 
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

void MainWindow::updateStatusBar()
{
    if ( m_positionLabel )
        m_positionLabel->setText( QString( "%1 %2" ).
        arg( tr( POSITION_STRING ) ).arg( m_position ) ); 

    if ( m_distanceLabel )
        m_distanceLabel->setText( QString( "%1 %2" )
        .arg( tr( DISTANCE_STRING ) ).arg( m_distance ) ); 
}

void MainWindow::openFile()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                            QString(), 
                            tr("All Supported Files (*.gpx *.kml);;GPS Data (*.gpx);;Google Earth KML (*.kml)"));

    if ( ! fileName.isNull() ) {
        QString extension = fileName.section( '.', -1 );

        if ( extension.compare( "gpx", Qt::CaseInsensitive ) == 0 ) {
            m_controlView->marbleWidget()->openGpxFile( fileName );
        }
        else if ( extension.compare( "kml", Qt::CaseInsensitive ) 
                  == 0 ) 
        {
            m_controlView->marbleWidget()->addPlacemarkFile( fileName );
        }
    }
}

void MainWindow::setupStatusBar()
{
    statusBar()->setSizeGripEnabled( true );

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

    connect( marbleWidget(), SIGNAL( mouseMoveGeoPosition( QString ) ),
              this, SLOT( showPosition( QString ) ) );
    connect( marbleWidget(), SIGNAL( distanceChanged( QString ) ),
              this, SLOT( showDistance( QString ) ) );

    updateStatusBar();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

QString MainWindow::readMarbleDataPath()
{
#ifdef Q_WS_MAC
     QSettings settings("KDE.org", "Marble Desktop Globe");
#else
     QSettings settings("KDE", "Marble Desktop Globe");
#endif

     settings.beginGroup("MarbleWidget");
         QString marbleDataPath;
         marbleDataPath = settings.value("marbleDataPath", "").toString(), 
     settings.endGroup();

     return marbleDataPath;
}

void MainWindow::readSettings()
{
#ifdef Q_WS_MAC
     QSettings settings("KDE.org", "Marble Desktop Globe");
#else
     QSettings settings("KDE", "Marble Desktop Globe");
#endif
     settings.beginGroup("MainWindow");
         resize(settings.value("size", QSize(640, 480)).toSize());
         move(settings.value("pos", QPoint(200, 200)).toPoint());
         showFullScreen(settings.value("fullScreen", false ).toBool());
         showSideBar(settings.value("sideBar", true ).toBool());
         showStatusBar(settings.value("statusBar", false ).toBool());
         show();
         showClouds(settings.value("showClouds", true ).toBool());
         workOffline(settings.value("workOffline", false ).toBool());
         showAtmosphere(settings.value("showAtmosphere", true ).toBool());
     settings.endGroup();

     setUpdatesEnabled(false);

     settings.beginGroup("MarbleWidget");
         QString mapThemeId = settings.value("mapTheme", "" ).toString();
         qDebug() << "ReadSettings: mapThemeId: " << mapThemeId;
         m_controlView->marbleWidget()->setMapThemeId( mapThemeId );
         m_controlView->marbleWidget()->setProjection(
            (Projection)(settings.value("projection", 0 ).toInt())
         );
     
         // Last location on quit
	 if ( m_configDialog->onStartup() == Marble::LastLocationVisited ) {
	    m_controlView->marbleWidget()->centerOn(
		settings.value("quitLongitude", 0.0).toDouble(),
		settings.value("quitLatitude", 0.0).toDouble() );
	    m_controlView->marbleWidget()->zoomView( settings.value("quitZoom", 1000).toInt() );
	 }

	 // Set home position
         m_controlView->marbleWidget()->setHome( 
            settings.value("homeLongitude", 9.4).toDouble(), 
            settings.value("homeLatitude", 54.8).toDouble(),
            settings.value("homeZoom", 1050 ).toInt()
         );
	 if ( m_configDialog->onStartup() == Marble::ShowHomeLocation ) {
	    m_controlView->marbleWidget()->goHome();
	 }
     settings.endGroup();
     
     settings.beginGroup( "Sun" );
         m_controlView->sunLocator()->setShow( settings.value( "showSun", false ).toBool() );
         m_controlView->sunLocator()->setCitylights( settings.value( "showCitylights", false ).toBool() );
         m_controlView->sunLocator()->setCentered( settings.value( "centerOnSun", false ).toBool() );
     settings.endGroup();

     setUpdatesEnabled(true);
     
     // The config dialog has to read settings.
     m_configDialog->readSettings();
}

void MainWindow::writeSettings()
{
#ifdef Q_WS_MAC
     QSettings settings("KDE.org", "Marble Desktop Globe");
#else
     QSettings settings("KDE", "Marble Desktop Globe");
#endif

     settings.beginGroup( "MainWindow" );
         settings.setValue( "size", size() );
         settings.setValue( "pos", pos() );
         settings.setValue( "fullScreen", m_fullScreenAct->isChecked() );
         settings.setValue( "sideBar", m_sideBarAct->isChecked() );
         settings.setValue( "statusBar", m_statusBarAct->isChecked() );
         settings.setValue( "showClouds", m_showCloudsAct->isChecked() );
         settings.setValue( "workOffline", m_workOfflineAct->isChecked() );
         settings.setValue( "showAtmosphere", m_showAtmosphereAct->isChecked() );
     settings.endGroup();

     settings.beginGroup( "MarbleWidget" );
         // Get the 'home' values from the widget and store them in the settings.
         qreal homeLon = 0;
         qreal homeLat = 0;
         int homeZoom = 0;
         m_controlView->marbleWidget()->home( homeLon, homeLat, homeZoom );
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
         int    quitZoom = m_controlView->marbleWidget()->zoom();

         settings.setValue( "quitLongitude", quitLon );
         settings.setValue( "quitLatitude", quitLat );
         settings.setValue( "quitZoom", quitZoom );
     settings.endGroup();
     
     settings.beginGroup( "Sun" );
         settings.setValue( "showSun",        m_controlView->sunLocator()->getShow() );
         settings.setValue( "showCitylights", m_controlView->sunLocator()->getCitylights() );
         settings.setValue( "centerOnSun",    m_controlView->sunLocator()->getCentered() );
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
    qDebug() << "Updating Settings ...";

    // FIXME: Font doesn't get updated instantly.
    m_controlView->marbleWidget()->setDefaultFont( m_configDialog->mapFont() );

    m_controlView->marbleWidget()->setMapQuality( m_configDialog->stillQuality(), Marble::Still );
    m_controlView->marbleWidget()->setMapQuality( m_configDialog->animationQuality(), Marble::Animation );

    m_controlView->marbleWidget()->setDefaultAngleUnit( m_configDialog->angleUnit() );
    MarbleGlobal::getInstance()->locale()->setDistanceUnit( m_configDialog->distanceUnit() );

    m_distance = m_controlView->marbleWidget()->distanceString();
    updateStatusBar();

    m_controlView->marbleWidget()->setAnimationsEnabled( m_configDialog->animateTargetVoyage() );

    // Cache
    m_controlView->marbleWidget()->setPersistentTileCacheLimit( m_configDialog->persistentTileCacheLimit() * 1024 );
    m_controlView->marbleWidget()->setVolatileTileCacheLimit( m_configDialog->volatileTileCacheLimit() * 1024 );

    m_controlView->marbleWidget()->setProxy( m_configDialog->proxyUrl(), m_configDialog->proxyPort(), m_configDialog->user(), m_configDialog->password() );

    m_controlView->marbleWidget()->updateChangedMap();
}

#include "QtMainWindow.moc"
