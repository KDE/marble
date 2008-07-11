//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Tobias Koenig  <tokoe@kde.org>"
//


// Own
#include "marble_part.h"

// Qt
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtGui/QClipboard>
#include <QtGui/QLabel>
#include <QtGui/QFontMetrics>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QPainter>
#include <QtGui/QStandardItemModel>

// KDE
#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kcomponentdata.h>
#include <kconfigdialog.h>
#include <kdeversion.h>
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
#include <kdeprintdialog.h>

// Marble library
#include "GeoDataPoint.h"
#include "lib/SunControlWidget.h"

// Local dir
#include "ui_MarbleViewSettingsWidget.h"
#include "ui_MarbleNavigationSettingsWidget.h"
#include "MarbleCacheSettingsWidget.h"
#include "MarblePluginSettingsWidget.h"

#include <MarbleDirs.h>
#include <ControlView.h>
#include "settings.h"

#include "MarbleAbstractFloatItem.h"

namespace
{
    const char* POSITION_STRING = I18N_NOOP( "Position: %1" );
    const char* DISTANCE_STRING = I18N_NOOP( "Altitude: %1" );
}

typedef KParts::GenericFactory< MarblePart > MarblePartFactory;
K_EXPORT_COMPONENT_FACTORY( libmarble_part, MarblePartFactory )

MarblePart::MarblePart( QWidget *parentWidget, QObject *parent, const QStringList &arguments )
  : KParts::ReadOnlyPart( parent ), 
    m_sunControlDialog( 0 ),
    m_pluginModel( 0 ),
    m_configDialog( 0 ),
    m_positionLabel( 0 ), 
    m_distanceLabel( 0 )
{
    // only set marble data path when a path was given
    if(arguments.count() != 0 && !arguments.first().isEmpty())
      MarbleDirs::setMarbleDataPath(arguments.first());

    setComponentData( MarblePartFactory::componentData() );

    m_controlView = new ControlView( parentWidget );
    setWidget( m_controlView );

    setupActions();

    readSettings();

    setXMLFile( "marble_part.rc" );

    m_statusBarExtension = new KParts::StatusBarExtension( this );

    m_position = NOT_AVAILABLE;
    m_distance = m_controlView->marbleWidget()->distanceString();

    QTimer::singleShot( 0, this, SLOT( setupStatusBar() ) );
}

MarblePart::~MarblePart()
{
    writeSettings();
    delete m_pluginModel;
    delete m_configDialog;
}

ControlView* MarblePart::controlView() const
{
    return m_controlView;
}

KAboutData *MarblePart::createAboutData()
{
  	return new KAboutData( I18N_NOOP( "marble_part" ), 0, ki18n( "A Desktop Globe" ), "0.6" );
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
            format = (char*)"JPG";
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

    QPrintDialog *printDialog = KdePrint::createPrintDialog(&printer, widget());

    if ( printDialog->exec() ) {

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
    delete printDialog;
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

void MarblePart::showFullScreen( bool isChecked )
{
    if ( isChecked ) {
        if ( KApplication::activeWindow() )
//  TODO: Deprecate alternative once KDE 4.0 is outdated
#if KDE_VERSION >= KDE_MAKE_VERSION(4, 0, 60)
            KToggleFullScreenAction::setFullScreen( KApplication::activeWindow(), true );
#else
	    KApplication::activeWindow()->setWindowState( KApplication::activeWindow()->windowState() | Qt::WindowFullScreen );
#endif
	;
    }
    else {
        if ( KApplication::activeWindow() )
//  TODO: Deprecate alternative once KDE 4.0 is outdated
#if KDE_VERSION >= KDE_MAKE_VERSION(4, 0, 60)
            KToggleFullScreenAction::setFullScreen( KApplication::activeWindow(), false );
#else
	    KApplication::activeWindow()->setWindowState( KApplication::activeWindow()->windowState() & ~Qt::WindowFullScreen );
#endif
	;
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

void MarblePart::showSun()
{
    if (!m_sunControlDialog) 
        m_sunControlDialog = new SunControlWidget( NULL, m_controlView->sunLocator() );


    m_sunControlDialog->show();
    m_sunControlDialog->raise();
    m_sunControlDialog->activateWindow();
}


void MarblePart::copyMap()
{
    QPixmap      mapPixmap = m_controlView->mapScreenShot();
    QClipboard  *clipboard = KApplication::clipboard();

    clipboard->setPixmap( mapPixmap );
}

void MarblePart::copyCoordinates()
{
    double lon = m_controlView->marbleWidget()->centerLongitude();
    double lat = m_controlView->marbleWidget()->centerLatitude();

    QString  positionString = GeoDataPoint( lon, lat, 0.0, GeoDataPoint::Degree ).toString();
    QClipboard  *clipboard = QApplication::clipboard();

    clipboard->setText( positionString );
}

void MarblePart::readSettings()
{ 
    // Last location on quit
    if ( MarbleSettings::onStartup() == Marble::LastLocationVisited ) {
        m_controlView->marbleWidget()->centerOn( 
            MarbleSettings::quitLongitude(),
            MarbleSettings::quitLatitude()
        );
        m_controlView->marbleWidget()->zoomView(
            MarbleSettings::quitZoom()
        );
    }

    // Set home position
    m_controlView->marbleWidget()->setHome( 
        MarbleSettings::homeLongitude(),
        MarbleSettings::homeLatitude(),
        MarbleSettings::homeZoom()
    );
    if ( MarbleSettings::onStartup() == Marble::ShowHomeLocation ) {
        m_controlView->marbleWidget()->goHome();
    }

    // Set map quality
    m_controlView->marbleWidget()->setMapQuality( (Marble::MapQuality) MarbleSettings::stillQuality(), Marble::Still );
    m_controlView->marbleWidget()->setMapQuality( (Marble::MapQuality) MarbleSettings::animationQuality(), Marble::Animation );

    // Set angle unit
    m_controlView->marbleWidget()->setDefaultAngleUnit( (Marble::AngleUnit) MarbleSettings::angleUnit() );

    // Set default font
    m_controlView->marbleWidget()->setDefaultFont( MarbleSettings::mapFont() );     

    // Set whether animations to the target are enabled
    m_controlView->marbleWidget()->setAnimationsEnabled( 
        MarbleSettings::animateTargetVoyage()
    );

    // Map theme and projection
    m_controlView->marbleWidget()->setMapThemeId( MarbleSettings::mapTheme() );
    m_controlView->marbleWidget()->setProjection( (Projection) MarbleSettings::projection() );

    m_controlView->marbleWidget()->setShowClouds( MarbleSettings::showClouds() );
    m_showCloudsAction->setChecked( MarbleSettings::showClouds() );
    m_controlView->marbleWidget()->setShowAtmosphere( MarbleSettings::showAtmosphere() );
    m_showAtmosphereAction->setChecked( MarbleSettings::showAtmosphere() );

    // Caches
    m_controlView->marbleWidget()->setVolatileTileCacheLimit( 1000 * MarbleSettings::volatileTileCacheLimit() );
    m_controlView->marbleWidget()->setPersistentTileCacheLimit( 1000 * MarbleSettings::persistentTileCacheLimit() );

    // Proxy
    m_controlView->marbleWidget()->setProxy( MarbleSettings::proxyUrl(),
                                             MarbleSettings::proxyPort() );

    // Plugins
    QHash<QString, int> pluginEnabled;

    int nameIdSize = MarbleSettings::pluginNameId().size();
    int enabledSize = MarbleSettings::pluginEnabled().size();

    if ( nameIdSize == enabledSize )
    {
        for ( int i = 0; i < enabledSize; ++i ) 
        {
            pluginEnabled[ MarbleSettings::pluginNameId()[i] ] = MarbleSettings::pluginEnabled()[i];
        }
    }

    QList<MarbleAbstractLayer *> pluginList = m_controlView->marbleWidget()->layerPlugins();    
    QList<MarbleAbstractLayer *>::const_iterator i;
    for (i = pluginList.constBegin(); i != pluginList.constEnd(); ++i)
    {
        if ( pluginEnabled.contains( (*i)->nameId() ) )
        {
            (*i)->setEnabled( pluginEnabled[ (*i)->nameId() ] );
            (*i)->item()->setCheckState( pluginEnabled[ (*i)->nameId() ]  ?  Qt::Checked : Qt::Unchecked );
        }
    }

    slotUpdateSettings();
}

void MarblePart::writeSettings()
{
    double  quitLon = m_controlView->marbleWidget()->centerLongitude();
    double  quitLat = m_controlView->marbleWidget()->centerLatitude();
    int     quitZoom = m_controlView->marbleWidget()->zoom();

    // Get the 'home' values from the widget and store them in the settings.
    MarbleSettings::setQuitLongitude( quitLon );
    MarbleSettings::setQuitLatitude( quitLat );
    MarbleSettings::setQuitZoom( quitZoom );

    double  homeLon = 0;
    double  homeLat = 0;
    int     homeZoom = 0;

    // Get the 'home' values from the widget and store them in the settings.
    m_controlView->marbleWidget()->home( homeLon, homeLat, homeZoom );
    MarbleSettings::setHomeLongitude( homeLon );
    MarbleSettings::setHomeLatitude( homeLat );
    MarbleSettings::setHomeZoom( homeZoom );

    // Set default font
    MarbleSettings::setMapFont( m_controlView->marbleWidget()->defaultFont() );

    // Get whether animations to the target are enabled
    MarbleSettings::setAnimateTargetVoyage( m_controlView->marbleWidget()->animationsEnabled() );

    m_controlView->marbleWidget()->home( homeLon, homeLat, homeZoom );

    // Map theme and projection
    MarbleSettings::setMapTheme( m_controlView->marbleWidget()->mapThemeId() );
    MarbleSettings::setProjection( m_controlView->marbleWidget()->projection() );

    MarbleSettings::setShowClouds( m_controlView->marbleWidget()->showClouds() );
    MarbleSettings::setShowAtmosphere( m_controlView->marbleWidget()->showAtmosphere() );

    MarbleSettings::setStillQuality( m_controlView->marbleWidget()->mapQuality( Marble::Still ) );
    MarbleSettings::setAnimationQuality( m_controlView->marbleWidget()->mapQuality( Marble::Animation )  );

    MarbleSettings::setAngleUnit( m_controlView->marbleWidget()->defaultAngleUnit() );

    // Caches
    MarbleSettings::setVolatileTileCacheLimit( m_controlView->marbleWidget()->volatileTileCacheLimit() / 1000 );
    MarbleSettings::setPersistentTileCacheLimit( m_controlView->marbleWidget()->persistentTileCacheLimit() / 1000 );

    // Proxy
    MarbleSettings::setProxyUrl( m_controlView->marbleWidget()->proxyHost() );
    MarbleSettings::setProxyPort( m_controlView->marbleWidget()->proxyPort() );

    QList<int> pluginEnabled;
    QStringList pluginNameId;

    QList<MarbleAbstractLayer *> pluginList = m_controlView->marbleWidget()->layerPlugins();    
    QList<MarbleAbstractLayer *>::const_iterator i;
    for (i = pluginList.constBegin(); i != pluginList.constEnd(); ++i)
    {
            pluginEnabled << static_cast<int>( (*i)->enabled() );
            pluginNameId  << (*i)->nameId();
    }
    MarbleSettings::setPluginEnabled( pluginEnabled );
    MarbleSettings::setPluginNameId(  pluginNameId );
    
    MarbleSettings::self()->writeConfig();
}

void MarblePart::setupActions()
{
    // Adding some actions for translation

    KAction action1(i18n("Open Map &Data..."), this);
    KAction action2(i18n("&Import Map Data..."), this);
    KAction action3(i18n("Scale &Bar"), this);

    KAction action4(i18n("&Add Place"), this);
    KAction action5(i18n("Panoramio Photos"), this);

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

    // Action: Copy Coordinates string
    m_copyCoordinatesAction = new KAction( this );
    actionCollection()->addAction( "edit_copy_coordinates", m_copyCoordinatesAction );
    m_copyCoordinatesAction->setText( i18n( "C&opy Coordinates" ) );
    connect( m_copyCoordinatesAction, SIGNAL( triggered( bool ) ), this, SLOT( copyCoordinates() ) );

    // Action: Open a Gpx or a Kml File
    m_openAct = KStandardAction::open( this, SLOT( openFile() ), actionCollection() );
    m_openAct->setText( i18n( "&Open Map..." ) );

    // Standard actions.  So far only Quit.
    KStandardAction::quit( kapp, SLOT( closeAllWindows() ), actionCollection() );

    // Action: Get hot new stuff
    m_newStuffAction = KNS::standardAction(QString(), this, SLOT(showNewStuffDialog()), actionCollection(), "new_stuff");
    // FIXME: Enable the name and status tip when string freeze is lifted.
#if 0
    m_newStuffAction = KNS::standardAction( i18n("Maps..."), this, SLOT(showNewStuffDialog()), actionCollection(), "new_stuff");
    m_newStuffAction->setStatusTip(i18n("&Download new maps"));
#endif
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

    // Action: Show Atmosphere option
    m_showAtmosphereAction = new KAction( this );
    actionCollection()->addAction( "show_atmosphere", m_showAtmosphereAction );
    m_showAtmosphereAction->setCheckable( true );
    m_showAtmosphereAction->setChecked( true );
    m_showAtmosphereAction->setText( i18n( "&Atmosphere" ) );
    connect( m_showAtmosphereAction, SIGNAL( triggered( bool ) ), this, SLOT( setShowAtmosphere( bool ) ) );

    // Action: Show Clouds option
    m_showCloudsAction = new KAction( this );
    actionCollection()->addAction( "show_clouds", m_showCloudsAction );
    m_showCloudsAction->setCheckable( true );
    m_showCloudsAction->setChecked( true );
    m_showCloudsAction->setText( i18n( "&Clouds" ) );
    connect( m_showCloudsAction, SIGNAL( triggered( bool ) ), this, SLOT( setShowClouds( bool ) ) );

    // Action: Show Sunshade options
    m_showSunAct = new KAction( this );
    actionCollection()->addAction( "show_sun", m_showSunAct );
    m_showSunAct->setText( i18n( "S&un Control" ) );
    connect( m_showSunAct, SIGNAL( triggered( bool ) ), this, SLOT( showSun() ) );

    KStandardAction::preferences( this, SLOT( editSettings() ), actionCollection() );

    readSettings();
}

void MarblePart::createInfoBoxesMenu()
{
    QList<MarbleAbstractFloatItem *> floatItemList = m_controlView->marbleWidget()->floatItems();

    QList<QAction*> actionList;

    QList<MarbleAbstractFloatItem *>::const_iterator i;
    for (i = floatItemList.constBegin(); i != floatItemList.constEnd(); ++i)
    {
        actionList.append( (*i)->action() );
    }
    unplugActionList( "infobox_actionlist" );
    plugActionList( "infobox_actionlist", actionList );
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
             this,                          SLOT( showPosition( QString ) ) );
    connect( m_controlView->marbleWidget(), SIGNAL( distanceChanged( QString ) ),
             this,                          SLOT( showDistance( QString ) ) );

    updateStatusBar();
}

void MarblePart::showNewStuffDialog()
{
    QString  newStuffConfig = KStandardDirs::locate ( "data", 
                                                      "marble/marble.knsrc" );
    kDebug() << "KNS config file:" << newStuffConfig;

    KNS::Engine  engine;
    bool         ret = engine.init( newStuffConfig );
    if ( ret ) {
        KNS::Entry::List entries = engine.downloadDialogModal(0);
    }

    // Update the map theme widget by updating the model.
    // Shouldn't be needed anymore ...
//    m_controlView->marbleControl()->updateMapThemes();
}

void MarblePart::editSettings()
{
    if ( KConfigDialog::showDialog( "settings" ) )
        return; 
 
        m_configDialog = new KConfigDialog( m_controlView, "settings", MarbleSettings::self() ); 

        // view page
        Ui_MarbleViewSettingsWidget ui_viewSettings;
        QWidget *w_viewSettings = new QWidget( 0 );
        w_viewSettings->setObjectName( "view_page" );
        ui_viewSettings.setupUi( w_viewSettings );
        m_configDialog->addPage( w_viewSettings, i18n( "View" ), "preferences-view" );

        // navigation page
        Ui_MarbleNavigationSettingsWidget ui_navigationSettings;
        QWidget *w_navigationSettings = new QWidget( 0 );
        w_navigationSettings->setObjectName( "navigation_page" );
        ui_navigationSettings.setupUi( w_navigationSettings );
        m_configDialog->addPage( w_navigationSettings, i18n( "Navigation" ), "preferences-navigation" );

        // cache page
        MarbleCacheSettingsWidget *w_cacheSettings = 
            new MarbleCacheSettingsWidget();
        w_cacheSettings->setObjectName( "cache_page" );
        m_configDialog->addPage( w_cacheSettings, i18n( "Cache & Proxy" ), "preferences-cache" );
        connect( w_cacheSettings, SIGNAL( clearVolatileCache() ), m_controlView->marbleWidget(), SLOT( clearVolatileTileCache() ) );
        connect( w_cacheSettings, SIGNAL( clearPersistentCache() ), m_controlView->marbleWidget(), SLOT( clearPersistentTileCache() ) );

        // plugin page

        m_pluginModel = new QStandardItemModel();
        QStandardItem *parentItem = m_pluginModel->invisibleRootItem();

        QList<MarbleAbstractLayer *> pluginList = m_controlView->marbleWidget()->layerPlugins();    
        QList<MarbleAbstractLayer *>::const_iterator i;
        for (i = pluginList.constBegin(); i != pluginList.constEnd(); ++i)
        {
            parentItem->appendRow( (*i)->item() );
        }

        MarblePluginSettingsWidget *w_pluginSettings = 
            new MarblePluginSettingsWidget();
        w_pluginSettings->setModel( m_pluginModel );
        w_pluginSettings->setObjectName( "plugin_page" );
        m_configDialog->addPage( w_pluginSettings, i18n( "Plugins" ), "preferences-plugin" );

        connect( w_pluginSettings, SIGNAL( pluginListViewClicked() ), SLOT( slotEnableButtonApply() ) );
        connect( m_configDialog, SIGNAL( settingsChanged( const QString &) ), SLOT( slotUpdateSettings() ) );
        connect( m_configDialog, SIGNAL( applyClicked() ), SLOT( slotApply() ) );
        connect( m_configDialog, SIGNAL( okClicked() ), SLOT( slotApply() ) );
        connect( m_configDialog, SIGNAL( cancelClicked() ), SLOT( slotCancel() ) );

        m_configDialog->show();
}

void MarblePart::slotEnableButtonApply()
{
        m_configDialog->enableButtonApply( true );
}

void MarblePart::slotApply()
{
    QList<MarbleAbstractLayer *> pluginList = m_controlView->marbleWidget()->layerPlugins();    
    QList<MarbleAbstractLayer *>::const_iterator i;
    for (i = pluginList.constBegin(); i != pluginList.constEnd(); ++i)
    {
        (*i)->applyItemState();
    }
}

void MarblePart::slotCancel()
{
    QList<MarbleAbstractLayer *> pluginList = m_controlView->marbleWidget()->layerPlugins();    
    QList<MarbleAbstractLayer *>::const_iterator i;
    for (i = pluginList.constBegin(); i != pluginList.constEnd(); ++i)
    {
        (*i)->retrieveItemState();
    }
}

void MarblePart::slotUpdateSettings()
{
    qDebug() << "Updating Settings ...";

    m_controlView->marbleWidget()->setDefaultFont( MarbleSettings::mapFont() );     

    m_controlView->marbleWidget()->setMapQuality( (Marble::MapQuality) MarbleSettings::stillQuality(), Marble::Still );
    m_controlView->marbleWidget()->setMapQuality( (Marble::MapQuality) MarbleSettings::animationQuality(), Marble::Animation );

    m_controlView->marbleWidget()->setDefaultAngleUnit( (Marble::AngleUnit) MarbleSettings::angleUnit() );

    m_controlView->marbleWidget()->setAnimationsEnabled( MarbleSettings::animateTargetVoyage() );

    m_controlView->marbleWidget()->setPersistentTileCacheLimit( MarbleSettings::persistentTileCacheLimit() * 1000 );
    m_controlView->marbleWidget()->setVolatileTileCacheLimit( MarbleSettings::volatileTileCacheLimit() * 1000 );

    m_controlView->marbleWidget()->setProxy( MarbleSettings::proxyUrl(),
                                             MarbleSettings::proxyPort() );

    m_controlView->marbleWidget()->updateChangedMap();
}

#include "marble_part.moc"
