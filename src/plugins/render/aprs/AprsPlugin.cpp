//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Claudiu Covaci <claudiu.covaci@gmail.com>
//

#include "AprsPlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>

#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"

using namespace Marble;

AprsPlugin::AprsPlugin()
{
}

QStringList AprsPlugin::backendTypes() const
{
    return QStringList( "aprs" );
}

QString AprsPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList AprsPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString AprsPlugin::name() const
{
    return tr( "Aprs Plugin" );
}

QString AprsPlugin::guiString() const
{
    return tr( "&Aprs Plugin" );
}

QString AprsPlugin::nameId() const
{
    return QString( "aprs-plugin" );
}

QString AprsPlugin::description() const
{
    return tr( "This plugin displays APRS data gleaned from the internet." );
}

QIcon AprsPlugin::icon () const
{
    return QIcon();
}


void AprsPlugin::initialize ()
{
}

bool AprsPlugin::isInitialized () const
{
    return true;
}

bool AprsPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    painter->autoMapQuality();

    GeoDataCoordinates spot(-120.103, 38.8067, 0, GeoDataCoordinates::Degree);

    painter->setPen( QColor( 255, 0, 0, 255 ) );

    painter->drawRect( spot, 30, 30 );
    painter->drawText( spot, "test" );

    GeoDataCoordinates home(-121.699, 38.555, 0, GeoDataCoordinates::Degree);
    painter->drawLine(spot, home);

    return true;
}

Q_EXPORT_PLUGIN2(AprsPlugin, Marble::AprsPlugin)

#include "AprsPlugin.moc"
