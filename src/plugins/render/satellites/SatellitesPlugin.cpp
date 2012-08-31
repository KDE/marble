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
#include "OrbiterSatellitesItem.h"
#include "SatellitesConfigLeafItem.h"
#include "SatellitesConfigNodeItem.h"
#include "SatellitesConfigModel.h"
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
      m_configDialog( 0 )
{
}

SatellitesPlugin::SatellitesPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
     m_earthSatModel( 0 ),
     m_orbiterSatModel( 0 ),
     m_isInitialized( false ),
     m_configDialog( new SatellitesConfigDialog() )
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
    return tr(
        "Earth-Satellites orbital elements from <ul><li>"
        "<a href=\"http://www.celestrak.com\">http://www.celestrak.com</a>"
        "</li></ul>"
        "Planetary-Satellites orbital elements from <ul><li>"
        "<a href=\"http://tasc.esa.eu/\">ESA TASC service</a></li><li>"
        "<a href=\"http://http://ssd.jpl.nasa.gov/?horizons\">"
        "JPL Horizons</a></li></ul>" );
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
    // FIXME: remove the const_cast, it may be best to create a new type of
    // plugins where marbleModel() is not const, since traditional
    // RenderPlugins do not require that

    m_earthSatModel = new EarthSatellitesModel(
        const_cast<MarbleModel *>( marbleModel() )->treeModel(),
        marbleModel()->pluginManager(),
        marbleModel()->clock() );

    m_configModel = new SatellitesConfigModel( this );
    m_configDialog->configWidget()->treeView->setModel( m_configModel );

    m_orbiterSatModel = new OrbiterSatellitesModel(
        const_cast<MarbleModel *>( marbleModel() )->treeModel(),
        marbleModel()->pluginManager(),
        marbleModel()->clock() );

    connect( m_earthSatModel, SIGNAL( fileParsed( const QString& ) ),
        SLOT( dataSourceParsed( const QString& ) ) );
    connect( m_orbiterSatModel, SIGNAL( fileParsed( const QString& ) ),
        SLOT( dataSourceParsed( const QString& ) ) );
    connect( m_orbiterSatModel, SIGNAL( itemUpdateEnded() ),
        SLOT( updateOrbiterConfig() ) );
    connect( m_configDialog, SIGNAL( reloadDataSourcesRequested() ),
        SLOT( updateSettings() ) );
    connect( m_configDialog, SIGNAL( accepted() ), SLOT( writeSettings() ) );
    connect( m_configDialog, SIGNAL( rejected() ), SLOT( readSettings() ) );
    connect( m_configDialog->configWidget()->buttonBox->button(
        QDialogButtonBox::Reset ),
        SIGNAL( clicked() ), SLOT( restoreDefaultSettings() ) );
    connect( m_configDialog, SIGNAL( userDataSourcesChanged() ),
        SLOT( writeSettings() ) );
    connect( m_configDialog, SIGNAL( userDataSourceAdded( const QString& ) ),
        SLOT( userDataSourceAdded( const QString& ) ) );

    m_isInitialized = true;
    readSettings();
    updateSettings();
    enableModel( enabled() );
}

bool SatellitesPlugin::isInitialized() const
{
    return m_isInitialized;
}

bool SatellitesPlugin::render( GeoPainter *painter, ViewportParams *viewport,
    const QString &renderPos, GeoSceneLayer *layer )
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

    // add default data sources
    if( !m_settings.contains( "dataSources" ) ) {
        QStringList dsList;
        dsList << "http://www.celestrak.com/NORAD/elements/visual.txt";
        m_settings.insert( "dataSources", dsList );
    } else if( m_settings.value( "dataSources" ).type() == QVariant::String ) {
        // HACK: KConfig can't guess the type of the settings, when we use
        // KConfigGroup::readEntry() in marble_part it returns a QString which
        // is then wrapped into a QVariant when added to the settings hash.
        // QVariant can handle the conversion for some types, like toDateTime()
        // but when calling toStringList() on a QVariant::String, it will
        // return a one element list
        m_settings.insert( "dataSources",
            m_settings.value( "dataSources" ).toString().split( "," ) );
    }

    // add default user data source
    if( !m_settings.contains( "userDataSources" ) ) {
        QStringList udsList;
        udsList << "http://www.bitquirl.net/~rene/esasocis/orbitdata/satcatalogue.msc";
        m_settings.insert( "userDataSources", udsList );
    } else if( m_settings.value( "userDataSources" ).type() == QVariant::String ) {
        m_settings.insert( "userDataSources",
            m_settings.value( "userDataSources" ).toString().split( "," ) );
    }

    emit settingsChanged( nameId() );
}

void SatellitesPlugin::readSettings()
{
    m_configDialog->setUserDataSources(
        m_settings.value( "userDataSources" ).toStringList() );
    m_configModel->loadSettings( m_settings );
}

void SatellitesPlugin::writeSettings()
{
    m_settings.insert( "userDataSources", m_configDialog->userDataSources() );
    m_settings.insert( "dataSources", m_configModel->urlList() );
    m_settings.insert( "idList", m_configModel->idList() );

    emit settingsChanged( nameId() );
}

void SatellitesPlugin::updateSettings()
{
    if (!isInitialized()) {
        return;
    }

    m_orbiterSatModel->clear();
    m_earthSatModel->clear();
    
    m_configModel->clear();
    addBuiltInDataSources();

    // data sources
    QStringList dsList = m_settings["dataSources"].toStringList();
    dsList << m_settings["userDataSources"].toStringList();
    foreach( const QString &ds, dsList ) {
        mDebug() << "Loading satellite data from:" << ds;
        if( ds.section( '/', -1 ).endsWith( ".txt" ) ) {
            m_earthSatModel->downloadFile( QUrl( ds ), ds );
        } else {
            m_orbiterSatModel->downloadFile( QUrl( ds ), ds );
        }
    }
}

void SatellitesPlugin::dataSourceParsed( const QString &source )
{
    m_configDialog->setUserDataSourceLoaded( source, true );
}

void SatellitesPlugin::userDataSourceAdded( const QString &source )
{
    // initially enable sources (and their items)
    QStringList idList = m_settings["idList"].toStringList();
    QStringList dsList = m_settings["dataSources"].toStringList();
    idList << source;
    dsList << source;
    m_settings.insert( "idList", idList );
    m_settings.insert( "dataSources", dsList );
}

SatellitesConfigDialog *SatellitesPlugin::configDialog()
{
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
    } else {
        m_earthSatModel->enable( false );
    }

    m_orbiterSatModel->setPlanet( marbleModel()->planetId() );
    m_orbiterSatModel->enable( enabled && visible() );
}

void SatellitesPlugin::visibleModel( bool visible )
{
    if ( !m_isInitialized ) {
        return;
    }

    if( marbleModel()->planetId() == "earth" )
    {
        m_earthSatModel->enable( enabled() && visible );
    } else {
        m_earthSatModel->enable( false );
    }

    m_orbiterSatModel->setPlanet( marbleModel()->planetId() );
    m_orbiterSatModel->enable( enabled() && visible );
}

void SatellitesPlugin::updateOrbiterConfig()
{
    mDebug() << "Updating orbiter configuration";

    foreach( QObject *obj, m_orbiterSatModel->items() ) {
        OrbiterSatellitesItem *item = qobject_cast<OrbiterSatellitesItem*>( obj );
        if( item != NULL ) {
            m_configDialog->addSatelliteItem(
                item->relatedBody(),
                item->category(),
                item->name(),
                item->id() );
        }
    }

    readSettings();

    m_configDialog->update();
}

void SatellitesPlugin::addBuiltInDataSources()
{
    const char *desc = "A category of satellites to be displayed";
    QString currentCategory;

    currentCategory = tr("Special-Interest Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Last 30 Days' Launches", desc ), "http://www.celestrak.com/NORAD/elements/tle-new.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Space Stations", desc ), "http://www.celestrak.com/NORAD/elements/stations.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "100 (or so) Brightest", desc ), "http://www.celestrak.com/NORAD/elements/visual.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "FENGYUN 1C Debris", desc ), "http://www.celestrak.com/NORAD/elements/1999-025.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "IRIDIUM 33 Debris", desc ), "http://www.celestrak.com/NORAD/elements/iridium-33-debris.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "COSMOS 2251 Debris", desc ), "http://www.celestrak.com/NORAD/elements/cosmos-2251-debris.txt" );

    currentCategory = tr( "Weather & Earth Resources Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Weather", desc ), "http://www.celestrak.com/NORAD/elements/weather.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "NOAA", desc ), "http://www.celestrak.com/NORAD/elements/noaa.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "GOES", desc ), "http://www.celestrak.com/NORAD/elements/goes.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Earth Resources", desc ), "http://www.celestrak.com/NORAD/elements/resource.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Search & Rescue (SARSAT)", desc ), "http://www.celestrak.com/NORAD/elements/sarsat.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Disaster Monitoring", desc ), "http://www.celestrak.com/NORAD/elements/dmc.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Tracking and Data Relay Satellite System (TDRSS)", desc ), "http://www.celestrak.com/NORAD/elements/tdrss.txt" );

    currentCategory = tr( "Communications Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Geostationary", desc ), "http://www.celestrak.com/NORAD/elements/geo.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Intelsat", desc ), "http://www.celestrak.com/NORAD/elements/intelsat.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Gorizont", desc ), "http://www.celestrak.com/NORAD/elements/gorizont.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Raduga", desc ), "http://www.celestrak.com/NORAD/elements/raduga.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Molniya", desc ), "http://www.celestrak.com/NORAD/elements/molniya.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Iridium", desc ), "http://www.celestrak.com/NORAD/elements/iridium.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Orbcomm", desc ), "http://www.celestrak.com/NORAD/elements/orbcomm.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Globalstar", desc ), "http://www.celestrak.com/NORAD/elements/globalstar.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Amateur radio", desc ), "http://www.celestrak.com/NORAD/elements/amateur.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Experimental",desc ), "http://www.celestrak.com/NORAD/elements/x-comm.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Other", desc ), "http://www.celestrak.com/NORAD/elements/other-comm.txt" );

    currentCategory = tr( "Navigation Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "GPS Operational", desc ), "http://www.celestrak.com/NORAD/elements/gps-ops.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Glonass Operational", desc ), "http://www.celestrak.com/NORAD/elements/glo-ops.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Galileo", desc ), "http://www.celestrak.com/NORAD/elements/galileo.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Satellite-Based Augmentation System (WAAS/EGNOS/MSAS)", desc ), "http://www.celestrak.com/NORAD/elements/sbas.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Navy Navigation Satellite System (NNSS)", desc ), "http://www.celestrak.com/NORAD/elements/nnss.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Russian LEO Navigation", desc ), "http://www.celestrak.com/NORAD/elements/musson.txt" );

    currentCategory = tr( "Scientific Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Space & Earth Science", desc ), "http://www.celestrak.com/NORAD/elements/science.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Geodetic", desc ), "http://www.celestrak.com/NORAD/elements/geodetic.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Engineering", desc ), "http://www.celestrak.com/NORAD/elements/engineering.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Education", desc ), "http://www.celestrak.com/NORAD/elements/education.txt" );

    currentCategory = tr( "Miscellaneous Satellites" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Miscellaneous Military", desc ), "http://www.celestrak.com/NORAD/elements/military.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Radar Calibration", desc ), "http://www.celestrak.com/NORAD/elements/radar.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "CubeSats", desc ), "http://www.celestrak.com/NORAD/elements/cubesat.txt" );
    m_configDialog->addTLESatelliteItem( currentCategory, tr( "Other", desc ), "http://www.celestrak.com/NORAD/elements/other.txt" );

    readSettings();
    m_configDialog->update();
}

} // namespace Marble

Q_EXPORT_PLUGIN2( SatellitesPlugin, Marble::SatellitesPlugin )

#include "SatellitesPlugin.moc"

