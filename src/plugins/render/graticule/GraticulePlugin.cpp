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
    initLineMaps( GeoDataCoordinates::defaultNotation() );
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

    GeoDataCoordinates::Notation notation = GeoDataCoordinates::defaultNotation();
    if ( notation != m_currentNotation ) {
        initLineMaps( notation );
    }

    GeoDataLatLonAltBox viewLatLonAltBox = viewport->viewLatLonAltBox();

    // Render the normal grid

    qreal degreeStep = 360.0 / m_normalLineMap.lowerBound(viewport->radius()).value();

    renderLongitudeLines( painter, viewLatLonAltBox, degreeStep, degreeStep );  
    renderLatitudeLines(  painter, viewLatLonAltBox, degreeStep );  
    
    painter->setPen( QColor( Qt::yellow ) );


    // Render the equator
    renderLatitudeLine( painter, 0.0, viewLatLonAltBox.west(), viewLatLonAltBox.east() );

    // Render the Meridian and Antimeridian
    renderLongitudeLines( painter, viewLatLonAltBox, 90.0 );  


    // Render the tropics
    QPen graticulePen = painter->pen();
    graticulePen.setStyle( Qt::DotLine );        
    painter->setPen( graticulePen );

    qreal axialTilt = RAD2DEG * dataFacade()->planet()->epsilon();

    renderLatitudeLine( painter, +axialTilt, viewLatLonAltBox.west(), viewLatLonAltBox.east() );        
    renderLatitudeLine( painter, -axialTilt, viewLatLonAltBox.west(), viewLatLonAltBox.east() );        

    renderLatitudeLine( painter, +90.0 - axialTilt, viewLatLonAltBox.west(), viewLatLonAltBox.east() );        
    renderLatitudeLine( painter, -90.0 + axialTilt, viewLatLonAltBox.west(), viewLatLonAltBox.east() );        

    painter->restore();

    return true;
}

void GraticulePlugin::renderLatitudeLine( GeoPainter *painter, qreal latitude,
                                                qreal fromWestLon,
                                                qreal toEastLon )
{
    GeoDataLineString circle( Tessellate | RespectLatitudeCircle ) ;

    if ( fromWestLon < toEastLon ) {
        qreal step = ( toEastLon - fromWestLon ) / 4.0;

        for ( int i = 0; i < 5; ++i ) {
            GeoDataCoordinates n( fromWestLon + i * step, latitude, 0.0, GeoDataCoordinates::Degree );
            circle << n;        
        }
    }
    else {
        qreal step = ( +180.0 - toEastLon ) / 4.0;

        for ( int i = 0; i < 5; ++i ) {
            GeoDataCoordinates n( toEastLon + i * step, latitude, 0.0, GeoDataCoordinates::Degree );
            circle << n;        
        }

        step = ( +180 + fromWestLon ) / 4.0;

        for ( int i = 0; i < 5; ++i ) {
            GeoDataCoordinates n( -180 + i * step, latitude, 0.0, GeoDataCoordinates::Degree );
            circle << n;        
        }
    }

    painter->drawPolyline( circle );     
}

void GraticulePlugin::renderLongitudeLine( GeoPainter *painter, qreal longitude,
                                                 qreal cutOff,
                                                 qreal fromSouthLat,
                                                 qreal toNorthLat )
{
    qreal southLat = ( fromSouthLat > -90.0 + cutOff ) ? -90.0 + cutOff : fromSouthLat;
    qreal northLat = ( toNorthLat   > +90.0 - cutOff ) ? +90.0 - cutOff : toNorthLat;

    GeoDataCoordinates n1(longitude, southLat, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n3(longitude, northLat, 0.0, GeoDataCoordinates::Degree );

    GeoDataLineString halfCircle( Tessellate );

    if ( northLat > 0 && southLat < 0 )
    {
        GeoDataCoordinates n2( longitude, 0.0, 0.0, GeoDataCoordinates::Degree );
        halfCircle << n1 << n2 << n3;
    }
    else {
        halfCircle << n1 << n3;
    }

    painter->drawPolyline( halfCircle );     
}

void GraticulePlugin::renderLatitudeLines( GeoPainter *painter,
                                           const GeoDataLatLonAltBox& viewLatLonAltBox,
                                           qreal step )
{
//    qDebug() << "LatLonAlt:" << viewLatLonAltBox.toString(GeoDataCoordinates::Degree);

    // Latitude
    qreal southLat = viewLatLonAltBox.south(GeoDataCoordinates::Degree);
    qreal northLat = viewLatLonAltBox.north(GeoDataCoordinates::Degree);

    qreal southLineLat = step * static_cast<int>( southLat / step ); 
    qreal northLineLat = step * ( static_cast<int>( northLat / step ) + 1 );

    // Longitude
    qreal westLon = viewLatLonAltBox.west(GeoDataCoordinates::Degree);
    qreal eastLon = viewLatLonAltBox.east(GeoDataCoordinates::Degree);

    qreal westLineLon = step * static_cast<int>( westLon / step );
    qreal eastLineLon = step * ( static_cast<int>( eastLon / step ) + 1 ); 

//    qDebug() << "LatitudeBounds:" << southLat << northLat;
//    qDebug() << "LatitudeLines: " << southLineLat << northLineLat;

    qreal itStep = southLineLat;

    while ( itStep < northLineLat ) {
        renderLatitudeLine( painter, itStep, westLon, eastLon );
//        qDebug() << "Painted LatLine:" << itStep;
        itStep += step;
//        qDebug() << "Next LatLine:" << itStep;
    }
}

void GraticulePlugin::renderLongitudeLines( GeoPainter *painter, 
                                            const GeoDataLatLonAltBox& viewLatLonAltBox, 
                                            qreal step, qreal cutOff )
{
    // Latitude
    qreal southLat = viewLatLonAltBox.south(GeoDataCoordinates::Degree);
    qreal northLat = viewLatLonAltBox.north(GeoDataCoordinates::Degree);

    qreal southLineLat = step * static_cast<int>( southLat / step ); 
    qreal northLineLat = step * ( static_cast<int>( northLat / step ) + 1 );

    // Longitude
    qreal westLon = viewLatLonAltBox.west(GeoDataCoordinates::Degree);
    qreal eastLon = viewLatLonAltBox.east(GeoDataCoordinates::Degree);

    qreal westLineLon = step * static_cast<int>( westLon / step );
    qreal eastLineLon = step * ( static_cast<int>( eastLon / step ) + 1 ); 

//    qDebug() << "LongitudeLines:" << westLon << eastLon;

    if ( !viewLatLonAltBox.crossesDateLine() ) {
        qreal itStep = westLineLon;

        while ( itStep < eastLineLon ) {
            renderLongitudeLine( painter, itStep, cutOff, southLat, northLat );                
            itStep += step;
        }
    }
    else {
        qreal itStep = eastLineLon;

        while ( itStep < 180.0 ) {
            renderLongitudeLine( painter, itStep, cutOff, southLat, northLat );                
            itStep += step;
        }

        itStep = -180.0;
        while ( itStep < westLineLon ) {
            renderLongitudeLine( painter, itStep, cutOff );                
            itStep += step;
        }
    }
}

void GraticulePlugin::initLineMaps( GeoDataCoordinates::Notation notation)
{
    /* Define Upper Bound keys and associated values: */
    m_normalLineMap[90]     = 4;          // 90 deg
    m_normalLineMap[400]    = 12;          // 90 deg
    m_normalLineMap[650]   = 36;         // 30 deg
    m_normalLineMap[8000]   = 72;         // 10 deg
    m_normalLineMap[16000]  = 360;         //  5 deg
    m_normalLineMap[32000] = 720;        //  1 deg
    m_normalLineMap[64000] = 720;        //  0.5 deg = 30'

    switch ( notation )
    {
        case GeoDataCoordinates::Decimal :
            m_normalLineMap[64000]  = 360 * 10;       //  0.1 deg
            m_normalLineMap[256000] = 360 * 20;       //  0.05 deg
            m_normalLineMap[1024000] = 360 * 100;      //  0.01 deg
            m_normalLineMap[4096000] = 360 * 200;      //  0.005 deg
            m_normalLineMap[16384000] = 360 * 1000;    //  0.001 deg
            m_normalLineMap[65536000] = 360 * 2000;    //  0.0005 deg
            m_normalLineMap[262144000] = 360 * 10000;  //  0.00001 deg
        break;
        default:
        case GeoDataCoordinates::DMS :
            m_normalLineMap[64000]  = 360 * 6;         //  10'
            m_normalLineMap[256000] = 360 * 12;        //  5'
            m_normalLineMap[1024000] = 360 * 60;        //  1'
            m_normalLineMap[4096000] = 360 * 60 * 2;    //  30"
            m_normalLineMap[16384000] = 360 * 60 * 6;   //  10"
            m_normalLineMap[65536000] = 360 * 60 * 12;  //  5"
            m_normalLineMap[262144000] = 360 * 60 * 60; //  1"
        break;
    }
    m_normalLineMap[999999999] = m_normalLineMap.value(262144000);     //  last

    m_currentNotation = notation;
}

}

Q_EXPORT_PLUGIN2(GraticulePlugin, Marble::GraticulePlugin)

#include "GraticulePlugin.moc"
