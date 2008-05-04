//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleTestPlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>

#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataPoint.h"


QStringList MarbleTestPlugin::backendTypes() const
{
    return QStringList( "test" );
}

QString MarbleTestPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QString MarbleTestPlugin::renderPosition() const
{
    return QString( "ALWAYS_ON_TOP" );
}

QString MarbleTestPlugin::name() const
{
    return QString( "Test Plugin" );
}

QString MarbleTestPlugin::nameId() const
{
    return QString( "test-plugin" );
}

QString MarbleTestPlugin::description() const
{
    return QString( "This is a simple test plugin." );
}

QIcon MarbleTestPlugin::icon () const
{
    return QIcon();
}


void MarbleTestPlugin::initialize ()
{
}

bool MarbleTestPlugin::isInitialized () const
{
    return true;
}

bool MarbleTestPlugin::render( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer )
{
    GeoDataPoint flensburg( 9.4, 54.8, 0.0, GeoDataPoint::Degree );
    GeoDataPoint linkoeping( 15.6, 58.4, 0.0, GeoDataPoint::Degree );
    GeoDataPoint istanbul( 28.0, 41.0, 0.0, GeoDataPoint::Degree );
    GeoDataPoint orbit( 105.6, 30.0, 3000000.0, GeoDataPoint::Degree );

    painter->setPen( QColor( 99, 198, 99, 255 ) );
    painter->setBrush( QColor( 99, 198, 99, 80 ) );
    painter->drawEllipse( flensburg, 30, 30 ); 

    painter->drawText( flensburg, "Torsten" );

    painter->setPen( QColor( 198, 99, 99, 255 ) );
    painter->setBrush( QColor( 198, 99, 99, 80 ) );
    painter->drawEllipse( linkoeping, 40, 40 ); 

    painter->drawText( linkoeping, "Inge" );

    painter->setPen( QColor( 99, 99, 198, 255 ) );
    painter->setBrush( QColor( 99, 99, 198, 80 ) );
    painter->drawEllipse( orbit, 20, 20 ); 

    painter->drawText( orbit, "Claudiu" );

    painter->drawPixmap( istanbul, QPixmap( MarbleDirs::path( "bitmaps/earth_apollo.jpg" ) ) ); 

    return true;
}

Q_EXPORT_PLUGIN2(MarbleTestPlugin, MarbleTestPlugin)

#include "MarbleTestPlugin.moc"
