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

#include "GeoDataLatLonAltBox.h"

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
    return QStringList( "SURFACE" ); // although this is not a float item we choose the position of one
}

QString GraticulePlugin::name() const
{
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
    return tr( "A plugin that shows a coordinate grid." );
}

QIcon GraticulePlugin::icon () const
{
    return QIcon();
}

void GraticulePlugin::initialize ()
{
    // Initialize range maps that map the zoom to the number of coordinate grid lines.
    initLineMaps();
}

bool GraticulePlugin::isInitialized () const
{
    return true;
}

bool GraticulePlugin::render( GeoPainter *painter, ViewportParams *viewport,
				const QString& renderPos,
				GeoSceneLayer * layer )
{
    if ( renderPos != "SURFACE" ) {
        return true;
    }

    painter->save();

    painter->autoMapQuality();

    painter->setPen( QColor( Qt::white ) );

/*
    // TESTCASE
    for ( int i = -90; i < 90 ; i+=3 ){
        renderLatitudeCircle( painter, i );
    }

    for ( int i = -180; i < 180.0 ; i+=3 ){
        renderLongitudeHalfCircle( painter, i );                
    }
*/

    qreal degreeStep = 360.0 / m_normalLineMap.lowerBound(viewport->radius()).value();

    qDebug() << "STEP" << degreeStep;

    GeoDataLatLonAltBox viewLatLonAltBox = viewport->viewLatLonAltBox();

    renderLongitudeLines( painter, viewLatLonAltBox, degreeStep );  
    renderLatitudeLines(  painter, viewLatLonAltBox, degreeStep );  
    
    painter->setPen( QColor( Qt::yellow ) );

    renderLatitudeCircle( painter, 0.0 );

    renderLongitudeCircle( painter, 0.0 );        
    renderLongitudeCircle( painter, 90.0 );        


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

    return true;
}

void GraticulePlugin::renderLatitudeCircle( GeoPainter *painter, qreal latitude )
{
    GeoDataCoordinates n1(-180.0, latitude, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n2(-90.0, latitude, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n3(0.0, latitude, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n4(+90.0, latitude, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n5(+180.0, latitude, 0.0, GeoDataCoordinates::Degree );

    GeoDataLineString circle( Tessellate | RespectLatitudeCircle ) ;

    circle << n1 << n2 << n3 << n4 << n5;

    painter->drawPolyline( circle );     
}

void GraticulePlugin::renderLongitudeCircle( GeoPainter *painter, qreal longitude  )
{
    GeoDataCoordinates n1(longitude, 90.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n2(longitude, 0.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n3(longitude, -90.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n4(longitude + 180.0, 0.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n5(longitude, 90.0, 0.0, GeoDataCoordinates::Degree );

    GeoDataLineString circle( Tessellate ) ;

    circle << n1 << n2 << n3 << n4 << n5;

    painter->drawPolyline( circle );     
}

void GraticulePlugin::renderLongitudeHalfCircle( GeoPainter *painter, qreal longitude, qreal cutOff )
{
    GeoDataCoordinates n1(longitude, 90.0 - cutOff, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n2(longitude, 0.0, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n3(longitude, -90.0 + cutOff, 0.0, GeoDataCoordinates::Degree );

    GeoDataLineString halfCircle( Tessellate ) ;

    halfCircle << n1 << n2 << n3;

    painter->drawPolyline( halfCircle );     
}

void GraticulePlugin::renderLatitudeLines( GeoPainter *painter,
                                           const GeoDataLatLonAltBox& viewLatLonAltBox,
                                           qreal step )
{
    // Latitude
    qreal southLat = viewLatLonAltBox.south(GeoDataCoordinates::Degree);
    qreal northLat = viewLatLonAltBox.north(GeoDataCoordinates::Degree);

    qreal southLineLat = step * static_cast<int>( southLat / step ); 
    qreal northLineLat = step * static_cast<int>( northLat / step );

    // Longitude
    qreal westLon = viewLatLonAltBox.west(GeoDataCoordinates::Degree);
    qreal eastLon = viewLatLonAltBox.east(GeoDataCoordinates::Degree);

    qreal westLineLon = step * static_cast<int>( westLon / step );
    qreal eastLineLon = step * static_cast<int>( eastLon / step ); 

//    qDebug() << "LatitudeLines:" << southLat << northLat;

    qreal itStep = southLineLat;

    while ( itStep <= northLineLat ) {
        renderLatitudeCircle( painter, itStep );
        itStep += step;
    }
}

void GraticulePlugin::renderLongitudeLines( GeoPainter *painter, 
                                            const GeoDataLatLonAltBox& viewLatLonAltBox, 
                                            qreal step )
{
    // Latitude
    qreal southLat = viewLatLonAltBox.south(GeoDataCoordinates::Degree);
    qreal northLat = viewLatLonAltBox.north(GeoDataCoordinates::Degree);

    qreal southLineLat = step * static_cast<int>( southLat / step ); 
    qreal northLineLat = step * static_cast<int>( northLat / step );

    // Longitude
    qreal westLon = viewLatLonAltBox.west(GeoDataCoordinates::Degree);
    qreal eastLon = viewLatLonAltBox.east(GeoDataCoordinates::Degree);

    qreal westLineLon = step * static_cast<int>( westLon / step );
    qreal eastLineLon = step * static_cast<int>( eastLon / step ); 

//    qDebug() << "LongitudeLines:" << westLon << eastLon;

    if ( !viewLatLonAltBox.crossesDateLine() ) {
        qreal itStep = westLineLon;

        while ( itStep <= eastLineLon ) {
            renderLongitudeHalfCircle( painter, itStep, step );                
            itStep += step;
        }
    }
    else {
        qreal itStep = eastLineLon;

        while ( itStep <= 180.0 ) {
            renderLongitudeHalfCircle( painter, itStep, step );                
            itStep += step;
        }

        itStep = -180.0;
        while ( itStep <= westLineLon ) {
            renderLongitudeHalfCircle( painter, itStep, step );                
            itStep += step;
        }
    }
}

void GraticulePlugin::initLineMaps()
{
    /* Define Upper Bound keys and associated values: */
    m_normalLineMap[90]    = 4;          // 90 deg
    m_normalLineMap[400]   = 12;         // 30 deg
    m_normalLineMap[650]   = 36;         // 10 deg
    m_normalLineMap[8000]  = 72;         //  5 deg
    m_normalLineMap[16000] = 360;        //  1 deg
    m_normalLineMap[32000] = 720;        //  0.5 deg = 30'

    GeoDataCoordinates::Notation notation = GeoDataCoordinates::defaultNotation();
    switch ( notation )
    {
        case GeoDataCoordinates::Decimal :
            m_normalLineMap[64000]  = 360 * 10;       //  0.1 deg
            m_normalLineMap[128000] = 360 * 20;       //  0.05 deg
            m_normalLineMap[512000] = 360 * 100;      //  0.01 deg
            m_normalLineMap[1024000] = 360 * 200;      //  0.005 deg
        break;
        default:
        case GeoDataCoordinates::DMS :
            m_normalLineMap[64000]  = 360 * 6;         //  10'
            m_normalLineMap[128000] = 360 * 12;        //  5'
            m_normalLineMap[512000] = 360 * 60;        //  1'
            m_normalLineMap[1024000] = 360 * 60 * 2;    //  30"
        break;
    }
    m_normalLineMap[99999999] = m_normalLineMap.value(1024000);     //  last
}

}

Q_EXPORT_PLUGIN2(GraticulePlugin, Marble::GraticulePlugin)

#include "GraticulePlugin.moc"
