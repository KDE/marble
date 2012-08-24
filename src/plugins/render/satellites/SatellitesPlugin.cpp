//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "SatellitesPlugin.h"

#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "GeoDataPlacemark.h"
#include "EarthSatellitesItem.h"
#include "SatellitesConfigLeafItem.h"
#include "SatellitesConfigNodeItem.h"
#include "EarthSatellitesConfigModel.h"
#include "OrbiterSatellitesConfigModel.h"
#include "ViewportParams.h"

#include "ui_SatellitesConfigDialog.h"

#include <QtCore/QUrl>
#include <QtGui/QPushButton>

namespace Marble
{

SatellitesPlugin::SatellitesPlugin()
    : RenderPlugin( 0 ),
      m_earthSatModel( 0 ),
      m_orbiterSatModel( 0 ),
      m_configDialog( 0 ),
      ui_configWidget( 0 )
{
}

SatellitesPlugin::SatellitesPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
     m_earthSatModel( 0 ),
     m_orbiterSatModel( 0 ),
     m_isInitialized( false ),
     m_configDialog( 0 ),
     m_earthSatConfigModel( 0 ),
     ui_configWidget( 0 )
{
    connect( this, SIGNAL(settingsChanged(QString)), SLOT(updateSettings()) );
    connect( this, SIGNAL(enabledChanged(bool)), SLOT(enableModel(bool)) );
    connect( this, SIGNAL(visibilityChanged(bool,QString)), SLOT(visibleModel(bool)) );

    setVisible( false );
    setSettings( QHash<QString, QVariant>() );
}

SatellitesPlugin::~SatellitesPlugin()
{
    delete m_earthSatModel;
    delete m_orbiterSatModel;

    delete m_configDialog;
    delete ui_configWidget;
}

QStringList SatellitesPlugin::backendTypes() const
{
    return QStringList( "satellites" );
}

QString SatellitesPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList SatellitesPlugin::renderPosition() const
{
    return QStringList( "ORBIT" );
}

QString SatellitesPlugin::name() const
{
    return tr( "Satellites" );
}

QString SatellitesPlugin::nameId() const
{
    return "satellites";
}

QString SatellitesPlugin::guiString() const
{
    return tr( "&Satellites" );
}

QString SatellitesPlugin::version() const
{
    return "2.0";
}

QString SatellitesPlugin::description() const
{
    return tr( "This plugin displays satellites and their orbits." );
}

QString SatellitesPlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> SatellitesPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Guillaume Martres", "smarter@ubuntu.com" )
            << PluginAuthor( "Rene Kuettner", "rene@bitkanal.net" );
}

QString SatellitesPlugin::aboutDataText() const
{
    return tr( "Earth-Satellites orbital elements from <a href=\"http://www.celestrak.com\">http://www.celestrak.com</a>" );
}

QIcon SatellitesPlugin::icon() const
{
    return QIcon();
}

RenderPlugin::RenderType SatellitesPlugin::renderType() const
{
    return Online;
}

void SatellitesPlugin::initialize()
{
    //FIXME: remove the const_cast, it may be best to create a new type of plugins where
    //marbleModel() is not const, since traditional RenderPlugins do not require that
    m_earthSatModel = new EarthSatellitesModel(
        const_cast<MarbleModel *>( marbleModel() )->treeModel(),
        marbleModel()->pluginManager(),
        marbleModel()->clock() );
    m_orbiterSatModel = new OrbiterSatellitesModel(
        const_cast<MarbleModel *>( marbleModel() )->treeModel(),
        marbleModel()->pluginManager(),
        marbleModel()->clock() );

    m_isInitialized = true;
    updateSettings();
    enableModel( enabled() );
}

bool SatellitesPlugin::isInitialized() const
{
    return m_isInitialized;
}

bool SatellitesPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( painter );
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    enableModel( enabled() );

    return true;
}

QHash<QString, QVariant> SatellitesPlugin::settings() const
{
    return m_settings;
}

void SatellitesPlugin::setSettings( const QHash<QString, QVariant> &settings )
{
    m_settings = settings;

    if ( !m_settings.contains( "tleList" ) ) {
        QStringList tleList;
        tleList << "http://www.celestrak.com/NORAD/elements/visual.txt";
        m_settings.insert( "tleList", tleList );
    } else if ( m_settings.value( "tleList" ).type() == QVariant::String ) {
        //HACK: KConfig can't guess the type of the settings, when we use KConfigGroup::readEntry()
        // in marble_part it returns a QString which is then wrapped into a QVariant when added
        // to the settings hash. QVariant can handle the conversion for some types, like toDateTime()
        // but when calling toStringList() on a QVariant::String, it will return a one element list
        m_settings.insert( "tleList", m_settings.value( "tleList" ).toString().split( "," ) );
    }

    if( !m_settings.contains( "catList" ) ) {
        QStringList catList;
        catList << "http://www.bitquirl.net/~rene/esasocis/oribtdata/satcatalogue.txt";
        m_settings.insert( "catList", catList );
    } else if ( m_settings.value( "catList" ).type() == QVariant::String ) {
        m_settings.insert( "catList", m_settings.value( "catList" ).toString().split( "," ) );
    }

    readSettings();
    emit settingsChanged( nameId() );
}

void SatellitesPlugin::readSettings()
{
    if ( !m_configDialog )
        return;

    m_earthSatConfigModel->loadSettings( m_settings );
}

void SatellitesPlugin::writeSettings()
{
    QStringList tleList = m_earthSatConfigModel->tleList();
    m_settings.insert( "tleList", tleList );

    emit settingsChanged( nameId() );
}

void SatellitesPlugin::updateSettings()
{
    if (!isInitialized()) {
        return;
    }

    m_earthSatModel->clear();
    m_orbiterSatModel->clear();

    // earth satellites
    QStringList tleList = m_settings["tleList"].toStringList();
    foreach( const QString &tle, tleList ) {
        mDebug() << "Loading tle:" << tle;
        m_earthSatModel->downloadFile( QUrl( tle ), tle.mid( tle.lastIndexOf( '/' ) + 1 ) );
    }

    // orbiter
    QStringList orbDatList = m_settings["orbiterDataList"].toStringList();
    foreach( const QString &orbData, orbDatList ) {
        qDebug() << "Adding orbiter:" << orbData;
        m_orbiterSatModel->addOrbiterFromCatalogLine( orbData );
    }
}

QDialog *SatellitesPlugin::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::SatellitesConfigDialog();
        ui_configWidget->setupUi( m_configDialog );

        m_earthSatConfigModel = new EarthSatellitesConfigModel( this );
        m_orbiterSatConfigModel = new OrbiterSatellitesConfigModel( this );

        setupConfigModel();

        // earth
        ui_configWidget->treeViewEarth->setModel( m_earthSatConfigModel );
        ui_configWidget->treeViewEarth->expandAll();
        for ( int i = 0; i < m_earthSatConfigModel->columnCount(); i++ ) {
            ui_configWidget->treeViewEarth->resizeColumnToContents( i );
        }

        connect( m_configDialog, SIGNAL(accepted()), SLOT(writeSettings()) );
        connect( m_configDialog, SIGNAL(rejected()), SLOT(readSettings()) );
        connect( ui_configWidget->buttonBox->button( QDialogButtonBox::Reset ),
                 SIGNAL(clicked()), SLOT(restoreDefaultSettings()) );

        // orbiter
        QPushButton *buttonUpdateCatalog;
        buttonUpdateCatalog = new QPushButton( tr( "&Update Object Catalog" ),
            ui_configWidget->buttonBoxCatalog );
        buttonUpdateCatalog->setDefault( false );
        ui_configWidget->buttonBoxCatalog->addButton(
            buttonUpdateCatalog, QDialogButtonBox::ActionRole );

        QPushButton *buttonOpenCatalog;
        buttonOpenCatalog = new QPushButton( tr( "&Open Catalog from Disk" ),
            ui_configWidget->buttonBoxCatalog );
        buttonOpenCatalog->setDefault( false );
        ui_configWidget->buttonBoxCatalog->addButton(
            buttonOpenCatalog, QDialogButtonBox::ActionRole );

        connect( buttonUpdateCatalog, SIGNAL( clicked() ),
                 SLOT( updateOrbiterCatalog() ) );
        connect( buttonOpenCatalog, SIGNAL( clicked() ),
                 SLOT( openCatalogFromDisk() ) );

        readSettings();

    }

    return m_configDialog;
}

void SatellitesPlugin::enableModel( bool enabled )
{
    if ( !m_isInitialized ) {
        return;
    }

    if( marbleModel()->planetId() == "earth" )
    {
        m_earthSatModel->enable( enabled && visible() );
        m_orbiterSatModel->enable( false );
    } else {
        m_earthSatModel->enable( false );
        m_orbiterSatModel->setPlanet( marbleModel()->planetId() );
        m_orbiterSatModel->enable( enabled && visible() );
    }
}

void SatellitesPlugin::visibleModel( bool visible )
{
    if ( !m_isInitialized ) {
        return;
    }

    if( marbleModel()->planetId() == "earth" )
    {
        m_earthSatModel->enable( enabled() && visible );
        m_orbiterSatModel->enable( false );
    } else {
        m_earthSatModel->enable( false );
        m_orbiterSatModel->setPlanet( marbleModel()->planetId() );
        m_orbiterSatModel->enable( enabled() && visible );
    }
}

void SatellitesPlugin::updateOrbiterCatalog()
{
    // catList
    QStringList catList = m_settings["catList"].toStringList();
    foreach( const QString &cat, catList ) {
        mDebug() << "Loading catalog:" << cat;
        m_orbiterSatModel->downloadFile( QUrl( cat ), cat.mid( cat.lastIndexOf( '/' ) + 1 ) );
    }
}

void SatellitesPlugin::setupConfigModel()
{
    const char *desc = "A category of satellites to be displayed";
    SatellitesConfigNodeItem *node = new SatellitesConfigNodeItem( tr("Special-Interest Satellites" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Last 30 Days' Launches", desc ), "http://www.celestrak.com/NORAD/elements/tle-new.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Space Stations", desc ), "http://www.celestrak.com/NORAD/elements/stations.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "100 (or so) Brightest", desc ), "http://www.celestrak.com/NORAD/elements/visual.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "FENGYUN 1C Debris", desc ), "http://www.celestrak.com/NORAD/elements/1999-025.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "IRIDIUM 33 Debris", desc ), "http://www.celestrak.com/NORAD/elements/iridium-33-debris.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "COSMOS 2251 Debris", desc ), "http://www.celestrak.com/NORAD/elements/cosmos-2251-debris.txt" ) );
    m_earthSatConfigModel->appendChild( node );

    node = new SatellitesConfigNodeItem( tr( "Weather & Earth Resources Satellites" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Weather", desc ), "http://www.celestrak.com/NORAD/elements/weather.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "NOAA", desc ), "http://www.celestrak.com/NORAD/elements/noaa.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "GOES", desc ), "http://www.celestrak.com/NORAD/elements/goes.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Earth Resources", desc ), "http://www.celestrak.com/NORAD/elements/resource.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Search & Rescue (SARSAT)", desc ), "http://www.celestrak.com/NORAD/elements/sarsat.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Disaster Monitoring", desc ), "http://www.celestrak.com/NORAD/elements/dmc.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Tracking and Data Relay Satellite System (TDRSS)", desc ), "http://www.celestrak.com/NORAD/elements/tdrss.txt" ) );
    m_earthSatConfigModel->appendChild( node );

    node = new SatellitesConfigNodeItem( tr( "Communications Satellites" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Geostationary", desc ), "http://www.celestrak.com/NORAD/elements/geo.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Intelsat", desc ), "http://www.celestrak.com/NORAD/elements/intelsat.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Gorizont", desc ), "http://www.celestrak.com/NORAD/elements/gorizont.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Raduga", desc ), "http://www.celestrak.com/NORAD/elements/raduga.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Molniya", desc ), "http://www.celestrak.com/NORAD/elements/molniya.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Iridium", desc ), "http://www.celestrak.com/NORAD/elements/iridium.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Orbcomm", desc ), "http://www.celestrak.com/NORAD/elements/orbcomm.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Globalstar", desc ), "http://www.celestrak.com/NORAD/elements/globalstar.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Amateur radio", desc ), "http://www.celestrak.com/NORAD/elements/amateur.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Experimental",desc ), "http://www.celestrak.com/NORAD/elements/x-comm.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Other", desc ), "http://www.celestrak.com/NORAD/elements/other-comm.txt" ) );
    m_earthSatConfigModel->appendChild( node );

    node = new SatellitesConfigNodeItem( tr( "Navigation Satellites" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "GPS Operational", desc ), "http://www.celestrak.com/NORAD/elements/gps-ops.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Glonass Operational", desc ), "http://www.celestrak.com/NORAD/elements/glo-ops.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Galileo", desc ), "http://www.celestrak.com/NORAD/elements/galileo.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Satellite-Based Augmentation System (WAAS/EGNOS/MSAS)", desc ), "http://www.celestrak.com/NORAD/elements/sbas.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Navy Navigation Satellite System (NNSS)", desc ), "http://www.celestrak.com/NORAD/elements/nnss.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Russian LEO Navigation", desc ), "http://www.celestrak.com/NORAD/elements/musson.txt" ) );
    m_earthSatConfigModel->appendChild( node );

    node = new SatellitesConfigNodeItem( tr( "Scientific Satellites" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Space & Earth Science", desc ), "http://www.celestrak.com/NORAD/elements/science.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Geodetic", desc ), "http://www.celestrak.com/NORAD/elements/geodetic.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Engineering", desc ), "http://www.celestrak.com/NORAD/elements/engineering.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Education", desc ), "http://www.celestrak.com/NORAD/elements/education.txt" ) );
    m_earthSatConfigModel->appendChild( node );

    node = new SatellitesConfigNodeItem( tr( "Miscellaneous Satellites" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Miscellaneous Military", desc ), "http://www.celestrak.com/NORAD/elements/military.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Radar Calibration", desc ), "http://www.celestrak.com/NORAD/elements/radar.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "CubeSats", desc ), "http://www.celestrak.com/NORAD/elements/cubesat.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( tr( "Other", desc ), "http://www.celestrak.com/NORAD/elements/other.txt" ) );
    m_earthSatConfigModel->appendChild( node );
}

} // namespace Marble

Q_EXPORT_PLUGIN2( SatellitesPlugin, Marble::SatellitesPlugin )

#include "SatellitesPlugin.moc"
