//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>"
//

#include "GraticulePlugin.h"

#include <QtCore/QDebug>
#include "GeoPainter.h"
#include "GeoDataLineString.h"
#include "MarbleDirs.h"
#include "Planet.h"
#include "MarbleDataFacade.h"

#include "ViewportParams.h"

namespace Marble
{

QStringList GraticulePlugin::backendTypes() const
{
    return QStringList( "graticule" );
}

QString GraticulePlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList GraticulePlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" ); // although this is not a float item we choose the position of one
}

QString GraticulePlugin::name() const
{
    // FIXME (once we are out of string freeze):
    return tr( "Coordinate Grid" );
}

QString GraticulePlugin::guiString() const
{
    return tr( "Coordinate &Grid" );
}

QString GraticulePlugin::nameId() const
{
    return QString( "graticule" );
}

QString GraticulePlugin::description() const
{
    return QString( "" ); // tr( "A plugin that shows a graticule." );
}

QIcon GraticulePlugin::icon () const
{
    return QIcon();
}

void GraticulePlugin::initialize ()
{
}

bool GraticulePlugin::isInitialized () const
{
    return true;
}

bool GraticulePlugin::render( GeoPainter *painter, ViewportParams *viewport,
				const QString& renderPos,
				GeoSceneLayer * layer )
{
    if ( renderPos == "ALWAYS_ON_TOP" ) {

        painter->save();

        painter->autoMapQuality();

        painter->setPen( QColor( Qt::yellow ) );

        renderLatitudeCircle( painter, 0.0 );

        qreal axialTilt = RAD2DEG * dataFacade()->planet()->epsilon();

        // Setting the pen
        QPen graticulePen = painter->pen();
        graticulePen.setStyle( Qt::DotLine );        
        painter->setPen( graticulePen );

        renderLatitudeCircle( painter, +axialTilt );        
        renderLatitudeCircle( painter, -axialTilt );        

        renderLatitudeCircle( painter, +90.0 - axialTilt );        
        renderLatitudeCircle( painter, -90.0 + axialTilt );        

        painter->restore();
    }
    return true;
}

void GraticulePlugin::renderLatitudeCircle( GeoPainter *painter, qreal latitude ) {

        GeoDataCoordinates n1(-180.0, latitude, 0.0, GeoDataCoordinates::Degree );
        GeoDataCoordinates n2(-90.0, latitude, 0.0, GeoDataCoordinates::Degree );
        GeoDataCoordinates n3(0.0, latitude, 0.0, GeoDataCoordinates::Degree );
        GeoDataCoordinates n4(+90.0, latitude, 0.0, GeoDataCoordinates::Degree );
        GeoDataCoordinates n5(+180.0, latitude, 0.0, GeoDataCoordinates::Degree );

        GeoDataLineString circle( 0, Tessellate | RespectLatitudeCircle ) ;

        circle << &n1 << &n2 << &n3 << &n4 << &n5;

        painter->drawPolyline( circle );     
}

}

Q_EXPORT_PLUGIN2(GraticulePlugin, Marble::GraticulePlugin)

#include "GraticulePlugin.moc"
