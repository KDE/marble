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
#include "GeoDataPlacemark.h"
#include "ViewportParams.h"

#include <QtCore/QUrl>
#include <QtGui/QPushButton>

namespace Marble
{

PlanetarySatellitesPlugin::PlanetarySatellitesPlugin()
    : RenderPlugin( 0 ),
      m_isInitialized( false )
{
}

PlanetarySatellitesPlugin::PlanetarySatellitesPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
     m_isInitialized( false )
{
    connect( this, SIGNAL(enabledChanged(bool)), SLOT(enableModel(bool)) );
    connect( this, SIGNAL(visibilityChanged(bool,QString)), SLOT(visibleModel(bool)) );

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
    return tr( "This plugin displays planetary satellites, space probes and their orbits." );
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
    return tr( "???" );
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
    m_isInitialized = true;
}

bool PlanetarySatellitesPlugin::isInitialized() const
{
    return m_isInitialized;
}

bool PlanetarySatellitesPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( painter );
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    if( marbleModel()->planetId() == "earth" ) {
        enableModel( false );
    } else {
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
}

void PlanetarySatellitesPlugin::visibleModel( bool visible )
{
    Q_UNUSED( visible );

    if( !m_isInitialized ) {
        return;
    }
}

}

Q_EXPORT_PLUGIN2( PlanetarySatellitesPlugin, Marble::PlanetarySatellitesPlugin )

#include "PlanetarySatellitesPlugin.moc"
