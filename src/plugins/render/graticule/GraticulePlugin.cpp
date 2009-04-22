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
#include <QtGui/QBrush>
#include "GeoPainter.h"
#include "GeoDataLineString.h"
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
    return QStringList( "SURFACE" );
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

    if ( m_currentNotation != GeoDataCoordinates::defaultNotation() ) {
        initLineMaps( GeoDataCoordinates::defaultNotation() );
    }

    GeoDataLatLonAltBox viewLatLonAltBox = viewport->viewLatLonAltBox();

/*
    // TESTCASE
    for ( int i = -90; i < 90 ; i+=3 ){
        renderLatitudeCircle( painter, i, viewLatLonAltBox );
    }

    for ( int i = -180; i < 180.0 ; i+=3 ){
        renderLongitudeHalfCircle( painter, i, viewLatLonAltBox );                
    }
*/

    // Setting the label font for the coordinate lines.
#ifdef Q_OS_MACX
    int defaultFontSize = 10;
#else
    int defaultFontSize = 8;
#endif

    QFont gridFont("Sans Serif");
    gridFont.setPointSize( defaultFontSize );    

    QFont prominentLineFont = gridFont;
    prominentLineFont.setBold( true );
    painter->setFont( prominentLineFont );

    // Render the normal grid

    painter->setPen( QColor( Qt::white ) );

    // calculate the angular distance between coordinate lines of the normal grid
    qreal normalDegreeStep = 360.0 / m_normalLineMap.lowerBound(viewport->radius()).value();

    renderLongitudeLines( painter, viewLatLonAltBox, 
                          normalDegreeStep, normalDegreeStep, 
                          Marble::LineStart );  
    renderLatitudeLines(  painter, viewLatLonAltBox, normalDegreeStep,
                          Marble::LineStart );  

    // Render some non-cut off longitude lines ..
    renderLongitudeLine( painter, 90.0, viewLatLonAltBox );
    renderLongitudeLine( painter, -90.0, viewLatLonAltBox );

    // Render the bold grid

    painter->setPen( QPen( QBrush( Qt::white ), 1.5 ) );

    // calculate the angular distance between coordinate lines of the bold grid
    qreal boldDegreeStep = 360.0 / m_boldLineMap.lowerBound(viewport->radius()).value();

    renderLongitudeLines( painter, viewLatLonAltBox, 
                          boldDegreeStep, normalDegreeStep,
                          Marble::NoLabel
                        );  
    renderLatitudeLines(  painter, viewLatLonAltBox, boldDegreeStep,                                    
                          Marble::NoLabel );  

    
    painter->setPen( QColor( Qt::yellow ) );

    // Render the equator
    renderLatitudeLine( painter, 0.0, viewLatLonAltBox, tr( "Equator" ) );

    // Render the Meridian and Antimeridian
    renderLongitudeLine( painter, 0.0, viewLatLonAltBox, 0.0, tr( "Prime Meridian" ) );
    renderLongitudeLine( painter, 180.0, viewLatLonAltBox, 0.0, tr( "Antimeridian" ) );

    QPen graticulePen = painter->pen();
    graticulePen.setStyle( Qt::DotLine );        
    painter->setPen( graticulePen );

    // Determine the planet's axial tilt
    qreal axialTilt = RAD2DEG * dataFacade()->planet()->epsilon();

    // Render the tropics
    renderLatitudeLine( painter, +axialTilt, viewLatLonAltBox, tr( "Tropic of Cancer" )  );        
    renderLatitudeLine( painter, -axialTilt, viewLatLonAltBox, tr( "Tropic of Capricorn" ) );        

    // Render the arctics
    renderLatitudeLine( painter, +90.0 - axialTilt, viewLatLonAltBox, tr( "Arctic Circle" ) );        
    renderLatitudeLine( painter, -90.0 + axialTilt, viewLatLonAltBox, tr( "Antarctic Circle" ) );        

    painter->restore();

    return true;
}

void GraticulePlugin::renderLatitudeLine( GeoPainter *painter, qreal latitude,
                                          const GeoDataLatLonAltBox& viewLatLonAltBox,
                                          const QString& lineLabel,
                                          LabelPositionPolicy labelPositionPolicy )
{
    qreal fromSouthLat = viewLatLonAltBox.south( GeoDataCoordinates::Degree );
    qreal toNorthLat   = viewLatLonAltBox.north( GeoDataCoordinates::Degree );

    // Coordinate line is not displayed inside the viewport
    if ( latitude < fromSouthLat || toNorthLat < latitude ) {
        // qDebug() << "Lat: Out of View";
        return;
    }

    GeoDataLineString line( Tessellate | RespectLatitudeCircle ) ;

    qreal fromWestLon = viewLatLonAltBox.west( GeoDataCoordinates::Degree );
    qreal toEastLon   = viewLatLonAltBox.east( GeoDataCoordinates::Degree );

    if ( fromWestLon < toEastLon ) {
        qreal step = ( toEastLon - fromWestLon ) * 0.25;

        for ( int i = 0; i < 5; ++i ) {
            line << GeoDataCoordinates( fromWestLon + i * step, latitude, 0.0, GeoDataCoordinates::Degree );
        }
    }
    else {
        qreal step = ( +180.0 - toEastLon ) * 0.25;

        for ( int i = 0; i < 5; ++i ) {
            line << GeoDataCoordinates( toEastLon + i * step, latitude, 0.0, GeoDataCoordinates::Degree );
        }

        step = ( +180 + fromWestLon ) * 0.25;

        for ( int i = 0; i < 5; ++i ) {
            line << GeoDataCoordinates( -180 + i * step, latitude, 0.0, GeoDataCoordinates::Degree );
        }
    }

    painter->drawPolyline( line, lineLabel, labelPositionPolicy );     
}

void GraticulePlugin::renderLongitudeLine( GeoPainter *painter, qreal longitude,
                                           const GeoDataLatLonAltBox& viewLatLonAltBox, 
                                           qreal polarGap,
                                           const QString& lineLabel,
                                           LabelPositionPolicy labelPositionPolicy )
{
    qreal fromWestLon = viewLatLonAltBox.west( GeoDataCoordinates::Degree );
    qreal toEastLon   = viewLatLonAltBox.east( GeoDataCoordinates::Degree );

    // Coordinate line is not displayed inside the viewport
    if ( ( !viewLatLonAltBox.crossesDateLine() 
           && ( longitude < fromWestLon || toEastLon < longitude   ) ) ||
         (  viewLatLonAltBox.crossesDateLine() && 
            longitude < toEastLon && fromWestLon < longitude 
         ) 
       ) {
        // qDebug() << "Lon: Out of View";
        return;
    }

    qreal fromSouthLat = viewLatLonAltBox.south( GeoDataCoordinates::Degree );
    qreal toNorthLat   = viewLatLonAltBox.north( GeoDataCoordinates::Degree );
    
    qreal southLat = ( fromSouthLat < -90.0 + polarGap ) ? -90.0 + polarGap : fromSouthLat;
    qreal northLat = ( toNorthLat   > +90.0 - polarGap ) ? +90.0 - polarGap : toNorthLat;

    GeoDataCoordinates n1( longitude, southLat, 0.0, GeoDataCoordinates::Degree );
    GeoDataCoordinates n3( longitude, northLat, 0.0, GeoDataCoordinates::Degree );

    GeoDataLineString line( Tessellate );

    if ( northLat > 0 && southLat < 0 )
    {
        GeoDataCoordinates n2( longitude, 0.0, 0.0, GeoDataCoordinates::Degree );
        line << n1 << n2 << n3;
    }
    else {
        line << n1 << n3;
    }

    painter->drawPolyline( line, lineLabel, labelPositionPolicy );     
}

void GraticulePlugin::renderLatitudeLines( GeoPainter *painter,
                                           const GeoDataLatLonAltBox& viewLatLonAltBox,
                                           qreal step,
                                           LabelPositionPolicy labelPositionPolicy
                                         )
{
    if ( step <= 0 ) {
        return;
    }

    // Latitude
    qreal southLat = viewLatLonAltBox.south( GeoDataCoordinates::Degree );
    qreal northLat = viewLatLonAltBox.north( GeoDataCoordinates::Degree );

    qreal southLineLat = step * static_cast<int>( southLat / step ); 
    qreal northLineLat = step * ( static_cast<int>( northLat / step ) + 1 );

    qreal itStep = southLineLat;

    while ( itStep < northLineLat ) {
        // Create a matching label
        QString label = GeoDataCoordinates::latToString( itStep, 
                            GeoDataCoordinates::DMS, GeoDataCoordinates::Degree, 
                            -1, 'f' );

        // No additional labels for the equator
        if ( labelPositionPolicy == Marble::LineCenter && itStep == 0.0 ) {
            label.clear();
        }

        renderLatitudeLine( painter, itStep, viewLatLonAltBox, label, labelPositionPolicy );
        itStep += step;
    }
}

void GraticulePlugin::renderLongitudeLines( GeoPainter *painter, 
                                            const GeoDataLatLonAltBox& viewLatLonAltBox, 
                                            qreal step, qreal polarGap,
                                            LabelPositionPolicy labelPositionPolicy
                                           )
{
    if ( step <= 0 ) {
        return;
    }

    // Longitude
    qreal westLon = viewLatLonAltBox.west( GeoDataCoordinates::Degree );
    qreal eastLon = viewLatLonAltBox.east( GeoDataCoordinates::Degree );

    qreal westLineLon = step * static_cast<int>( westLon / step );
    qreal eastLineLon = step * ( static_cast<int>( eastLon / step ) + 1 ); 

    if ( !viewLatLonAltBox.crossesDateLine() ) {
        qreal itStep = westLineLon;

        while ( itStep < eastLineLon ) {
            // Create a matching label
            QString label = GeoDataCoordinates::lonToString( itStep, 
                                GeoDataCoordinates::DMS, GeoDataCoordinates::Degree, 
                                -1, 'f' );

            // No additional labels for the prime meridian and the antimeridian

            if ( labelPositionPolicy == Marble::LineCenter && ( itStep == 0.0 || itStep == 180.0 || itStep == -180.0 ) )
            {
                label.clear();
            }

            renderLongitudeLine( painter, itStep, viewLatLonAltBox, polarGap, 
                                 label, labelPositionPolicy );                
            itStep += step;
        }
    }
    else {
        qreal itStep = eastLineLon;

        while ( itStep < 180.0 ) {
            renderLongitudeLine( painter, itStep, viewLatLonAltBox, polarGap );                
            itStep += step;
        }

        itStep = -180.0;
        while ( itStep < westLineLon ) {
            renderLongitudeLine( painter, itStep, viewLatLonAltBox, polarGap );                
            itStep += step;
        }
    }
}

void GraticulePlugin::initLineMaps( GeoDataCoordinates::Notation notation)
{
    /* Define Upper Bound keys and associated values: */
    m_normalLineMap[90]     = 4;          // 90 deg
    m_normalLineMap[400]    = 12;          // 30 deg
    m_normalLineMap[650]   = 36;         // 10 deg
    m_normalLineMap[8000]   = 72;         // 5 deg
    m_normalLineMap[16000]  = 360;         //  1 deg
    m_normalLineMap[32000] = 720;        //  0.5 deg

    m_boldLineMap[650]     = 0;         //  1 deg
    m_boldLineMap[8000]    = 12;         //  1 deg
    m_boldLineMap[16000]   = 36;         //  1 deg

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

            m_boldLineMap[64000]     = 360;          // 0.1 deg
            m_boldLineMap[256000]    = 720;          // 0.05 deg
            m_boldLineMap[1024000]   = 360 * 10;     // 0.01 deg
            m_boldLineMap[4096000]   = 360 * 20;     // 0.005 deg
            m_boldLineMap[16384000]  = 360 * 100;    // 0.001 deg
            m_boldLineMap[65535000]  = 360 * 200;    // 0.0005 deg
            m_boldLineMap[262144000] = 360 * 1000;   // 0.00001 deg

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

            m_boldLineMap[64000]     = 360;          // 10'
            m_boldLineMap[256000]    = 720;          // 5'
            m_boldLineMap[1024000]   = 360 * 6;      // 1'
            m_boldLineMap[4096000]   = 360 * 12;     // 30"
            m_boldLineMap[16384000]  = 360 * 60;     // 10"
            m_boldLineMap[65535000]  = 360 * 60 * 2; // 5"
            m_boldLineMap[262144000] = 360 * 60 * 6; // 1"

        break;
    }
    m_normalLineMap[999999999] = m_normalLineMap.value(262144000);     //  last
    m_boldLineMap[999999999]   = m_boldLineMap.value(262144000);     //  last

    m_currentNotation = notation;
}

}

Q_EXPORT_PLUGIN2(GraticulePlugin, Marble::GraticulePlugin)

#include "GraticulePlugin.moc"
