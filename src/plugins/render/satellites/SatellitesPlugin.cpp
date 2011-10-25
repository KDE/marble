//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesPlugin.h"

#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "PluginAboutDialog.h"
#include "GeoDataPlacemark.h"
#include "SatellitesItem.h"
#include "SatellitesConfigLeafItem.h"
#include "SatellitesConfigModel.h"
#include "SatellitesConfigNodeItem.h"
#include "ViewportParams.h"

#include "sgp4/sgp4io.h"

#include "ui_SatellitesConfigDialog.h"

#include <QtCore/QUrl>
#include <QtGui/QPushButton>

namespace Marble
{

SatellitesPlugin::SatellitesPlugin()
    : RenderPlugin(),
     m_model( 0 ),
     m_isInitialized( false ),
     m_aboutDialog( 0 ),
     m_configDialog( 0 ),
     m_configModel( 0 ),
     ui_configWidget( 0 )
{
    connect( this, SIGNAL(settingsChanged(QString)), SLOT(updateSettings()) );

    setSettings( QHash<QString, QVariant>() );
}

SatellitesPlugin::~SatellitesPlugin()
{
    delete m_model;

    delete m_aboutDialog;
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

QString SatellitesPlugin::description() const
{
    return tr( "This plugin displays satellites and their orbits." );
}

QIcon SatellitesPlugin::icon() const
{
    return QIcon();
}

void SatellitesPlugin::initialize()
{
    //FIXME: remove the const_cast, it may be best to create a new type of plugins where
    //marbleModel() is not const, since traditional RenderPlugins do not require that
    m_model = new SatellitesModel( const_cast<MarbleModel *>( marbleModel() )->treeModel(), marbleModel()->pluginManager(),
                                   marbleModel()->clock() );
    m_isInitialized = true;
    updateSettings();
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

    return true;
}

QHash<QString, QVariant> SatellitesPlugin::settings() const
{
    return m_settings;
}

void SatellitesPlugin::setSettings( QHash<QString, QVariant> settings )
{
    if ( !settings.contains( "tleList" ) ) {
        QStringList tleList;
        tleList << "http://www.celestrak.com/NORAD/elements/visual.txt";
        settings.insert( "tleList", tleList );
    } else if ( settings.value( "tleList" ).type() == QVariant::String ) {
        //HACK: KConfig can't guess the type of the settings, when we use KConfigGroup::readEntry()
        // in marble_part it returns a QString which is then wrapped into a QVariant when added
        // to the settings hash. QVariant can handle the conversion for some types, like toDateTime()
        // but when calling toStringList() on a QVariant::String, it will return a one element list
        settings.insert( "tleList", settings.value( "tleList" ).toString().split( "," ) );
    }

    m_settings = settings;
    readSettings();
    emit settingsChanged( nameId() );
}

void SatellitesPlugin::readSettings()
{
    if ( !m_configDialog )
        return;

    QStringList tleList = m_settings.value( "tleList" ).toStringList();

    m_configModel->loadSettings( m_settings );
}

void SatellitesPlugin::writeSettings()
{
    QStringList tleList = m_configModel->tleList();

    m_settings.insert( "tleList", tleList );

    emit settingsChanged( nameId() );
}

void SatellitesPlugin::updateSettings()
{
    if (!isInitialized()) {
        return;
    }

    QStringList tleList = m_settings["tleList"].toStringList();
    foreach ( const QString &tle, tleList ) {
        mDebug() << tle;
        m_model->downloadFile( QUrl( tle ), tle.mid( tle.lastIndexOf( '/' ) + 1 ) );
    }
}

QDialog *SatellitesPlugin::aboutDialog()
{
    if ( !m_aboutDialog ) {
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Satellites Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        m_aboutDialog->setAboutText( tr( "<br />(c) 2011 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author gmartres;
        gmartres.name = "Guillaume Martres";
        gmartres.task = tr( "Developer" );
        gmartres.email = "smarter@ubuntu.com";
        authors.append( gmartres );
        m_aboutDialog->setAuthors( authors );
        m_aboutDialog->setDataText( tr( "Satellites orbital elements from <a href=\"http://www.celestrak.com\">http://www.celestrak.com</a>" ) );
        m_aboutDialog->setPixmap( icon().pixmap( 62, 53 ) );
    }
    return m_aboutDialog;
}

QDialog *SatellitesPlugin::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::SatellitesConfigDialog();
        ui_configWidget->setupUi( m_configDialog );

        m_configModel = new SatellitesConfigModel( this );

        setupConfigModel();

        ui_configWidget->treeView->setModel( m_configModel );
        ui_configWidget->treeView->expandAll();
        for ( int i = 0; i < m_configModel->columnCount(); i++ ) {
            ui_configWidget->treeView->resizeColumnToContents( i );
        }

        readSettings();

        connect( m_configDialog, SIGNAL(accepted()), SLOT(writeSettings()) );
        connect( m_configDialog, SIGNAL(rejected()), SLOT(readSettings()) );
        connect( ui_configWidget->buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()),
                 SLOT(restoreDefaultSettings()) );
    }

    return m_configDialog;
}

void SatellitesPlugin::setupConfigModel()
{
    SatellitesConfigNodeItem *node = new SatellitesConfigNodeItem( "Special-Interest Satellites" );
    node->appendChild( new SatellitesConfigLeafItem( "Last 30 Days' Launches", "http://www.celestrak.com/NORAD/elements/tle-new.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Space Stations", "http://www.celestrak.com/NORAD/elements/stations.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "100 (or so) Brightest", "http://www.celestrak.com/NORAD/elements/visual.txt") );
    node->appendChild( new SatellitesConfigLeafItem( "FENGYUN 1C Debris", "http://www.celestrak.com/NORAD/elements/1999-025.txt") );
    node->appendChild( new SatellitesConfigLeafItem( "IRIDIUM 33 Debris", "http://www.celestrak.com/NORAD/elements/iridium-33-debris.txt") );
    node->appendChild( new SatellitesConfigLeafItem( "COSMOS 2251 Debris", "http://www.celestrak.com/NORAD/elements/cosmos-2251-debris.txt") );
    m_configModel->appendChild( node );

    node = new SatellitesConfigNodeItem( "Weather & Earth Resources Satellites" );
    node->appendChild( new SatellitesConfigLeafItem( "Weather", "http://www.celestrak.com/NORAD/elements/weather.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "NOAA", "http://www.celestrak.com/NORAD/elements/noaa.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "GOES", "http://www.celestrak.com/NORAD/elements/goes.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Earth Resources", "http://www.celestrak.com/NORAD/elements/resource.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Search & Rescue (SARSAT)", "http://www.celestrak.com/NORAD/elements/sarsat.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Disaster Monitoring", "http://www.celestrak.com/NORAD/elements/dmc.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Tracking and Data Relay Satellite System (TDRSS)", "http://www.celestrak.com/NORAD/elements/tdrss.txt" ) );
    m_configModel->appendChild( node );

    node = new SatellitesConfigNodeItem( "Communications Satellites" );
    node->appendChild( new SatellitesConfigLeafItem( "Geostationary", "http://www.celestrak.com/NORAD/elements/geo.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Intelsat", "http://www.celestrak.com/NORAD/elements/intelsat.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Gorizont", "http://www.celestrak.com/NORAD/elements/gorizont.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Raduga", "http://www.celestrak.com/NORAD/elements/raduga.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Molniya", "http://www.celestrak.com/NORAD/elements/molniya.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Iridium", "http://www.celestrak.com/NORAD/elements/iridium.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Orbcomm", "http://www.celestrak.com/NORAD/elements/orbcomm.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Globalstar", "http://www.celestrak.com/NORAD/elements/globalstar.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Amateur radio", "http://www.celestrak.com/NORAD/elements/amateur.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Experimental", "http://www.celestrak.com/NORAD/elements/x-comm.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Other", "http://www.celestrak.com/NORAD/elements/other-comm.txt" ) );
    m_configModel->appendChild( node );

    node = new SatellitesConfigNodeItem( "Navigation Satellites" );
    node->appendChild( new SatellitesConfigLeafItem( "GPS Operational", "http://www.celestrak.com/NORAD/elements/gps-ops.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Glonass Operational", "http://www.celestrak.com/NORAD/elements/glo-ops.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Galileo", "http://www.celestrak.com/NORAD/elements/galileo.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Satellite-Based Augmentation System (WAAS/EGNOS/MSAS)", "http://www.celestrak.com/NORAD/elements/sbas.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Navy Navigation Satellite System (NNSS)", "http://www.celestrak.com/NORAD/elements/nnss.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Russian LEO Navigation", "http://www.celestrak.com/NORAD/elements/musson.txt" ) );
    m_configModel->appendChild( node );

    node = new SatellitesConfigNodeItem( "Scientific Satellites" );
    node->appendChild( new SatellitesConfigLeafItem( "Space & Earth Science", "http://www.celestrak.com/NORAD/elements/science.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Geodetic", "http://www.celestrak.com/NORAD/elements/geodetic.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Engineering", "http://www.celestrak.com/NORAD/elements/engineering.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Education", "http://www.celestrak.com/NORAD/elements/education.txt" ) );
    m_configModel->appendChild( node );

    node = new SatellitesConfigNodeItem( "Miscellaneous Satellites" );
    node->appendChild( new SatellitesConfigLeafItem( "Miscellaneous Military", "http://www.celestrak.com/NORAD/elements/military.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Radar Calibration", "http://www.celestrak.com/NORAD/elements/radar.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "CubeSats", "http://www.celestrak.com/NORAD/elements/cubesat.txt" ) );
    node->appendChild( new SatellitesConfigLeafItem( "Other", "http://www.celestrak.com/NORAD/elements/other.txt" ) );
    m_configModel->appendChild( node );
}

}

Q_EXPORT_PLUGIN2( SatellitesPlugin, Marble::SatellitesPlugin )

#include "SatellitesPlugin.moc"
