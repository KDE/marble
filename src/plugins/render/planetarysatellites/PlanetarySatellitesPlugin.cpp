//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "PlanetarySatellitesPlugin.h"

#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "ViewportParams.h"
#include "MarbleDirs.h"

#include "PlanetarySatellitesModel.h"

#include "mex/planetarySats.h"

namespace Marble
{

PlanetarySatellitesPlugin::PlanetarySatellitesPlugin()
    : RenderPlugin( 0 ),
      m_isInitialized( false )
{
}

PlanetarySatellitesPlugin::PlanetarySatellitesPlugin( const MarbleModel *model )
    : RenderPlugin( model ),
     m_isInitialized( false )
{
    connect( this,
             SIGNAL( enabledChanged( bool ) ),
             SLOT( enableModel( bool ) ) );
    connect( this,
             SIGNAL( visibilityChanged( bool, QString ) ),
             SLOT( visibleModel( bool ) ) );

    setVisible( false );
}

PlanetarySatellitesPlugin::~PlanetarySatellitesPlugin()
{
}

QStringList PlanetarySatellitesPlugin::backendTypes() const
{
    return QStringList( "planetarysatellites" );
}

QString PlanetarySatellitesPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList PlanetarySatellitesPlugin::renderPosition() const
{
    return QStringList( "ORBIT" );
}

QString PlanetarySatellitesPlugin::name() const
{
    return tr( "Planetary Satellites" );
}

QString PlanetarySatellitesPlugin::nameId() const
{
    return "planetarysatellites";
}

QString PlanetarySatellitesPlugin::guiString() const
{
    return tr( "&Planetary Satellites" );
}

QString PlanetarySatellitesPlugin::version() const
{
    return "1.0";
}

QString PlanetarySatellitesPlugin::description() const
{
    return tr( "This plugin displays planetary satellites and their orbits." );
}

QString PlanetarySatellitesPlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> PlanetarySatellitesPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Rene Kuettner", "rene@bitkanal.net" );
}

QString PlanetarySatellitesPlugin::aboutDataText() const
{
    // FIXME: add data about text
    return tr( "" );
}

QIcon PlanetarySatellitesPlugin::icon() const
{
    return QIcon();
}

RenderPlugin::RenderType PlanetarySatellitesPlugin::renderType() const
{
    return Online;
}

void PlanetarySatellitesPlugin::initialize()
{
    m_planSatModel = new PlanetarySatellitesModel(
        const_cast<MarbleModel *>( marbleModel() )->treeModel(),
        marbleModel()->pluginManager(), marbleModel()->clock() );

    m_isInitialized = true;

    enableModel( enabled() );
}

bool PlanetarySatellitesPlugin::isInitialized() const
{
    return m_isInitialized;
}

bool PlanetarySatellitesPlugin::render( GeoPainter *painter,
                                        ViewportParams *viewport,
                                        const QString &renderPos,
                                        GeoSceneLayer *layer )
{
    Q_UNUSED( painter );
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    if( marbleModel()->planetId() == "earth") {
        enableModel( false );
    } else {
        m_planSatModel->setPlanet( marbleModel()->planetId() );
        enableModel( enabled() );
    }

    return true;
}

void PlanetarySatellitesPlugin::enableModel( bool enabled )
{
    Q_UNUSED( enabled );

    if( !m_isInitialized ) {
        return;
    }

    m_planSatModel->enable( enabled && visible() );
}

void PlanetarySatellitesPlugin::visibleModel( bool visible )
{
    Q_UNUSED( visible );

    if( !m_isInitialized ) {
        return;
    }

    m_planSatModel->enable( enabled() && visible );
}

} // namespace Marble

Q_EXPORT_PLUGIN2( PlanetarySatellitesPlugin, Marble::PlanetarySatellitesPlugin )

#include "PlanetarySatellitesPlugin.moc"

