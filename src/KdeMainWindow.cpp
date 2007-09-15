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

#include "KdeMainWindow.h"

#include <QClipboard>
#include <QtGui/QLabel>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>

#include <KApplication>
#include <KIcon>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <KStatusBar>
#include <KMessageBox>
#include <KFileDialog>
#include <KToggleFullScreenAction>
#include <KPrinter>

#include <MarbleDirs.h>
#include "settings.h"

// #include <KPrintDialogPage>

namespace
{
    const char* POSITION_STRING = I18N_NOOP( "Position: %1" );
    const char* DISTANCE_STRING = I18N_NOOP( "Altitude: %1" );
}
 
MainWindow::MainWindow(const QString& marbleDataPath, QWidget *parent)
    : KXmlGuiWindow(parent), m_positionLabel( 0 ), m_distanceLabel( 0 )
{
    QString selectedPath = ( marbleDataPath.isEmpty() ) ? readMarbleDataPath() : marbleDataPath;
    if ( !selectedPath.isEmpty() )
        MarbleDirs::setMarbleDataPath( selectedPath );

    m_controlView = new ControlView( this );
    setCentralWidget( m_controlView );

    setupActions();
    setXMLFile("marbleui.rc");

    // Create the statusbar and populate it with initial data.
    m_statusBarExtension = new KParts::StatusBarExtension( this );

    m_position = NOT_AVAILABLE;
    m_distance = m_controlView->marbleWidget()->distanceString();

    QTimer::singleShot( 0, this, SLOT( setupStatusBar() ) );

    setAutoSaveSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
}


void MainWindow::setupActions()
{
    // Action: Print Map
    m_printMapAction = KStandardAction::print( this, SLOT( printMapScreenShot() ), actionCollection() );

    // Action: Export Map
    m_exportMapAction = new KAction( this );
    actionCollection()->addAction( "exportMap", m_exportMapAction );
    m_exportMapAction->setText( i18n( "&Export Map..." ) );
    m_exportMapAction->setIcon( KIcon( "document-save-as" ) );
    m_exportMapAction->setShortcut( Qt::CTRL + Qt::Key_S );
    connect( m_exportMapAction, SIGNAL(triggered( bool ) ),
             this,              SLOT( exportMapScreenShot() ) );

    // Action: Copy Map to the Clipboard
    m_copyMapAction = KStandardAction::copy( this, SLOT( copyMap() ), actionCollection() );
    m_copyMapAction->setText( i18n( "&Copy Map" ) );
    
    // Action: Open a Gpx or a Kml File
    m_openAct = KStandardAction::open( this, SLOT( openFile() ), actionCollection() );
    m_openAct->setText( i18n( "&Open Map..." ) );
//     m_openAct->setStatusTip( tr( "Open a file for viewing on
//             Marble"));

    // Standard actions.  So far only Quit.
    KStandardAction::quit( kapp, SLOT( closeAllWindows() ), actionCollection() );

    m_sideBarAct = new KAction( i18n("Show &Navigation Panel"), this );
    actionCollection()->addAction( "options_show_sidebar", m_sideBarAct );
    m_sideBarAct->setShortcut( Qt::Key_F9 );
    m_sideBarAct->setCheckable( true );
    m_sideBarAct->setChecked( true );
    m_sideBarAct->setStatusTip( i18n( "Show Navigation Panel" ) );
    connect( m_sideBarAct, SIGNAL( triggered( bool ) ), this, SLOT( showSideBar( bool ) ) );

    m_fullScreenAct = KStandardAction::fullScreen( 0, 0, this, actionCollection() );
    connect( m_fullScreenAct, SIGNAL( triggered( bool ) ), this, SLOT( showFullScreen( bool ) ) );

    setupGUI();

    readSettings();
}

void MarblePart::showPosition( const QString& position )
{
    m_position = position;
    updateStatusBar();
}

void MarblePart::showDistance( const QString& distance )
{
    m_distance = distance;
    updateStatusBar();
}

void MarblePart::updateStatusBar()
{
    if ( m_positionLabel )
        m_positionLabel->setText( i18n( POSITION_STRING, m_position ) ); 

    if ( m_distanceLabel )
        m_distanceLabel->setText( i18n( DISTANCE_STRING, m_distance ) ); 
}

void MainWindow::setupStatusBar()
{
    QFontMetrics statusBarFontMetrics( m_statusBarExtension->statusBar()->fontMetrics() );

    m_positionLabel = new QLabel( m_statusBarExtension->statusBar() );
    m_positionLabel->setIndent( 5 );
    QString templatePositionString = 
        QString( "%1 000\xb0 00\' 00\"_, 000\xb0 00\' 00\"_" ).arg(POSITION_STRING);
    int maxPositionWidth = statusBarFontMetrics.boundingRect(templatePositionString).width()
                            + 2 * m_positionLabel->margin() + 2 * m_positionLabel->indent();
    m_positionLabel->setFixedWidth( maxPositionWidth );
    m_statusBarExtension->addStatusBarItem( m_positionLabel, -1, false );

    m_distanceLabel = new QLabel( m_statusBarExtension->statusBar() );
    m_distanceLabel->setIndent( 5 );
    QString templateDistanceString = 
        QString( "%1 00.000,0 mu" ).arg(DISTANCE_STRING);
    int maxDistanceWidth = statusBarFontMetrics.boundingRect(templateDistanceString).width()
                            + 2 * m_distanceLabel->margin() + 2 * m_distanceLabel->indent();
    m_distanceLabel->setFixedWidth( maxDistanceWidth );
    m_statusBarExtension->addStatusBarItem( m_distanceLabel, -1, false );

    connect( m_controlView->marbleWidget(), SIGNAL( mouseMoveGeoPosition( QString ) ),
              this, SLOT( showPosition( QString ) ) );
    connect( m_controlView->marbleWidget(), SIGNAL( distanceChanged( QString ) ),
              this, SLOT( showDistance( QString ) ) );

    updateStatusBar();
}


void MainWindow::saveProperties( KConfigGroup &group )
{
    Q_UNUSED( group )
}


void MainWindow::readProperties( const KConfigGroup &group )
{
    Q_UNUSED( group )
}

QString MainWindow::readMarbleDataPath()
{
     return MarbleSettings::marbleDataPath();
}

void MainWindow::readSettings()
{
     m_controlView->marbleWidget()->setHome( 
        MarbleSettings::homeLongitude(),
        MarbleSettings::homeLatitude(),
        MarbleSettings::homeZoom()
     );
     m_controlView->marbleWidget()->goHome();
}


void MainWindow::writeSettings()
{
     double homeLon = 0;
     double homeLat = 0;
     int homeZoom = 0;
     m_controlView->marbleWidget()->home( homeLon, homeLat, homeZoom );
     MarbleSettings::setHomeLongitude( homeLon );
     MarbleSettings::setHomeLatitude( homeLat );
     MarbleSettings::setHomeZoom( homeZoom );
     MarbleSettings::self()->writeConfig();
}


void MainWindow::exportMapScreenShot()
{
    QString  fileName = KFileDialog::getSaveFileName( QDir::homePath(),
                                                      i18n( "Images (*.jpg *.png)" ),
                                                      this, i18n("Export Map") );

    if ( !fileName.isEmpty() ) {
        QPixmap mapPixmap = m_controlView->mapScreenShot();
        bool  success = mapPixmap.save( fileName );
        if ( !success ) {
            KMessageBox::error( this, i18nc( "Application name", "Marble" ),
                                i18n( "An error occurred while trying to save the file.\n" ),
                                KMessageBox::Notify );
        }
    }
}


void MainWindow::printMapScreenShot()
{
    QPixmap       mapPixmap = m_controlView->mapScreenShot();
    QSize         printSize = mapPixmap.size();
    KPrinter      printer;

    if ( printer.setup( this ) ) {

        QRect  mapPageRect = printer.pageRect();

        printSize.scale( ( printer.pageRect() ).size(), Qt::KeepAspectRatio );

        QPoint  printTopLeft( mapPageRect.x() + mapPageRect.width() / 2
                              - printSize.width() / 2,
                              mapPageRect.y() + mapPageRect.height() / 2
                              - printSize.height() / 2 );

        QRect     mapPrintRect( printTopLeft, printSize );
        QPainter  painter( &printer );

        painter.drawPixmap( mapPrintRect, mapPixmap, mapPixmap.rect() );
    }
}

void MainWindow::openFile()
{
    QString fileName;
    fileName = KFileDialog::getOpenFileName( KUrl(),
                                    i18n("*.gpx|GPS Data\n*.kml"),
                                            this, i18n("Open File")
                                           );/*
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                            QString(), 
                            tr("GPS Data(*.gpx);;KML(*.kml)"));*/

    if ( ! fileName.isNull() ) {
        QString extension = fileName.section( '.', -1 );

        if ( extension.compare( "gpx", Qt::CaseInsensitive ) == 0 ) {
            m_controlView->marbleWidget()->openGpxFile( fileName );
        }
        else if ( extension.compare( "kml", Qt::CaseInsensitive ) == 0 ) {
            m_controlView->marbleWidget()->addPlaceMarkFile( fileName );
        }
    }
}


#include "KdeMainWindow.moc"
