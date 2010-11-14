//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Claudiu Covaci <claudiu.covaci@gmail.com>
//

#include "SatellitesPlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>

#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

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

QString SatellitesPlugin::guiString() const
{
    return tr( "&Satellites" );
}

QString SatellitesPlugin::nameId() const
{
    return QString( "satellites-plugin" );
}

QString SatellitesPlugin::description() const
{
    return tr( "This plugin displays TBD satellites." );
}

QIcon SatellitesPlugin::icon () const
{
    return QIcon();
}


void SatellitesPlugin::initialize ()
{
}

bool SatellitesPlugin::isInitialized () const
{
    return true;
}

bool SatellitesPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    painter->autoMapQuality();

    GeoDataCoordinates flensburg( 9.4, 30.8, 2750000.0, GeoDataCoordinates::Degree );
    painter->setPen( QColor( 99, 198, 99, 255 ) );
    painter->setBrush( QColor( 99, 198, 99, 80 ) );
    painter->drawEllipse( flensburg, 30, 30 );
//     painter->drawPixmap( flensburg, QPixmap( MarbleDirs::path( "bitmaps/torsten.jpg" ) ) ); 
    painter->drawText( flensburg, "Torsten-air" );

    GeoDataCoordinates linkoeping( 15.6, 58.4, 2500000.0, GeoDataCoordinates::Degree );
    painter->setPen( QColor( 198, 99, 99, 255 ) );
    painter->setBrush( QColor( 198, 99, 99, 80 ) );
    painter->drawEllipse( linkoeping, 40, 40 ); 
//     painter->drawPixmap( linkoeping, QPixmap( MarbleDirs::path( "bitmaps/ingwa.jpg" ) ) ); 
    painter->drawText( linkoeping, "Inge-air" );

    GeoDataCoordinates orbit( 105.6, 0.0, 3000000.0, GeoDataCoordinates::Degree );
    painter->setPen( QColor( 99, 99, 198, 255 ) );
    painter->setBrush( QColor( 99, 99, 198, 80 ) );
    painter->drawEllipse( orbit, 20, 20 ); 
//     painter->drawPixmap( orbit, QPixmap( MarbleDirs::path( "bitmaps/ufo.jpeg" ) ) ); 
    painter->drawText( orbit, "Claudiu-air" );

    return true;
}

}

Q_EXPORT_PLUGIN2(SatellitesPlugin, Marble::SatellitesPlugin)

#include "SatellitesPlugin.moc"
