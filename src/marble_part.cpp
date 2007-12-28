//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig  <tokoe@kde.org>"
//

#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtGui/QClipboard>
#include <QtGui/QLabel>
#include <QtGui/QFontMetrics>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kcomponentdata.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kparts/genericfactory.h>
#include <kparts/statusbarextension.h>
#include <kstandardaction.h>
#include <kstatusbar.h>
#include <ktogglefullscreenaction.h>
#include <knewstuff2/ui/knewstuffaction.h>
#include <knewstuff2/engine.h>
#include <KStandardDirs>

#include <MarbleDirs.h>
#include <ControlView.h>
#include "settings.h"

#include "marble_part.h"

namespace
{
    const char* POSITION_STRING = I18N_NOOP( "Position: %1" );
    const char* DISTANCE_STRING = I18N_NOOP( "Altitude: %1" );
}

typedef KParts::GenericFactory< MarblePart > MarblePartFactory;
K_EXPORT_COMPONENT_FACTORY( libmarble_part, MarblePartFactory )

MarblePart::MarblePart( QWidget *parentWidget, QObject *parent, const QStringList &arguments )
  : KParts::ReadOnlyPart( parent ), m_positionLabel( 0 ), m_distanceLabel( 0 )
{
    // only set marble data path when a path was given
    if(arguments.count() != 0 && !arguments.first().isEmpty())
      MarbleDirs::setMarbleDataPath(arguments.first());

    setComponentData( MarblePartFactory::componentData() );

    m_controlView = new ControlView( parentWidget );
    setWidget( m_controlView );

    setupActions();

    setXMLFile( "marble_part.rc" );

    m_statusBarExtension = new KParts::StatusBarExtension( this );

    m_position = NOT_AVAILABLE;
    m_distance = m_controlView->marbleWidget()->distanceString();

    QTimer::singleShot( 0, this, SLOT( setupStatusBar() ) );

    readSettings();
}

MarblePart::~MarblePart()
{
    writeSettings();
}

ControlView* MarblePart::controlView() const
{
    return m_controlView;
}

KAboutData *MarblePart::createAboutData()
{
  	return new KAboutData( I18N_NOOP( "marble_part" ), 0, ki18n( "A Desktop Globe" ), "0.1" );
}

bool MarblePart::openUrl( const KUrl &url )
{
    Q_UNUSED( url );

    return true;
}

bool MarblePart::openFile()
{
    QString fileName;
    fileName = KFileDialog::getOpenFileName( KUrl(),
                                    i18n("*.gpx *.kml|All Supported Files\n*.gpx|GPS Data\n*.kml|Google Earth KML"),
                                            widget(), i18n("Open File")
                                           );
    if ( ! fileName.isNull() ) {
        QString extension = fileName.section( '.', -1 );

        if ( extension.compare( "gpx", Qt::CaseInsensitive ) == 0 ) {
            m_controlView->marbleWidget()->openGpxFile( fileName );
        }
        else if ( extension.compare( "kml", Qt::CaseInsensitive ) == 0 ) {
            m_controlView->marbleWidget()->addPlaceMarkFile( fileName );
        }
    }

    return true;
}


void MarblePart::exportMapScreenShot()
{
    QString  fileName = KFileDialog::getSaveFileName( QDir::homePath(),
                                                      i18n( "Images (*.jpg *.png)" ),
                                                      widget(), i18n("Export Map") );

    if ( !fileName.isEmpty() ) {
        // Take the case into account where no file format is indicated
        char * format = 0;
        if ( !fileName.endsWith("png", Qt::CaseInsensitive) 
           | !fileName.endsWith("jpg", Qt::CaseInsensitive) )
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


void MarblePart::printMapScreenShot()
{
    QPixmap       mapPixmap = m_controlView->mapScreenShot();
    QSize         printSize = mapPixmap.size();
    QPrinter      printer;

    QPrintDialog printDialog( &printer, widget() );

    if ( printDialog.exec() ) {

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

void MarblePart::showFullScreen( bool isChecked )
{
    if ( isChecked ) {
        if ( KApplication::activeWindow() )
            KApplication::activeWindow()->showFullScreen();
    }
    else {
        if ( KApplication::activeWindow() )
            KApplication::activeWindow()->showNormal();
    }

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

void MarblePart::copyMap()
{
    QPixmap      mapPixmap = m_controlView->mapScreenShot();
    QClipboard  *clipboard = KApplication::clipboard();

    clipboard->setPixmap( mapPixmap );
}

void MarblePart::readSettings()
{
    m_controlView->marbleWidget()->setHome( 
        MarbleSettings::homeLongitude(),
        MarbleSettings::homeLatitude(),
        MarbleSettings::homeZoom()
    );
    m_controlView->marbleWidget()->goHome();

    m_controlView->marbleWidget()->setProjection( MarbleSettings::projection() );
}

void MarblePart::writeSettings()
{
    double  homeLon = 0;
    double  homeLat = 0;
    int     homeZoom = 0;

    // Get the 'home' values from the widget...
    m_controlView->marbleWidget()->home( homeLon, homeLat, homeZoom );

    // ...and store them in the settings.
    MarbleSettings::setHomeLongitude( homeLon );
    MarbleSettings::setHomeLatitude( homeLat );
    MarbleSettings::setHomeZoom( homeZoom );

    MarbleSettings::setProjection( m_controlView->marbleWidget()->projection() );

    MarbleSettings::self()->writeConfig();
}

void MarblePart::setupActions()
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

    // Standard actions.  So far only Quit.
    KStandardAction::quit( kapp, SLOT( closeAllWindows() ), actionCollection() );

    // Action: Get hot new stuff
    m_newStuffAction = KNS::standardAction(QString(), this, SLOT(showNewStuffDialog()), actionCollection(), "new_stuff");
///@todo enable the name and status tip when string freeze is lifted
//     m_newStuffAction = KNS::standardAction(i18n("Maps..."), this, SLOT(showNewStuffDialog()), actionCollection(), "new_stuff");
//     m_newStuffAction->setStatusTip(i18n("&Download new maps"));
    m_newStuffAction->setShortcut( Qt::CTRL + Qt::Key_N );

    KStandardAction::showStatusbar( this, SLOT( showStatusBar( bool ) ), actionCollection() );

    m_sideBarAct = new KAction( i18n("Show &Navigation Panel"), this );
    actionCollection()->addAction( "options_show_sidebar", m_sideBarAct );
    m_sideBarAct->setShortcut( Qt::Key_F9 );
    m_sideBarAct->setCheckable( true );
    m_sideBarAct->setChecked( true );
    m_sideBarAct->setStatusTip( i18n( "Show Navigation Panel" ) );
    connect( m_sideBarAct, SIGNAL( triggered( bool ) ), this, SLOT( showSideBar( bool ) ) );

    m_fullScreenAct = KStandardAction::fullScreen( 0, 0, widget(), actionCollection() );
    connect( m_fullScreenAct, SIGNAL( triggered( bool ) ), this, SLOT( showFullScreen( bool ) ) );

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

void MarblePart::setupStatusBar()
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

void MarblePart::showNewStuffDialog()
{
    QString newStuffConfig = KStandardDirs::locate ("data", "marble/marble.knsrc");
    kDebug() << "KNS config file:" << newStuffConfig;

    KNS::Engine engine;
    bool ret = engine.init(newStuffConfig);
    if(ret)
    {
        KNS::Entry::List entries = engine.downloadDialogModal(0);
    }
}

#include "marble_part.moc"
