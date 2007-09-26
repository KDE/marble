//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include <QtCore/QDataStream>
#include <QtCore/QSize>
#include <QtGui/QPixmap>

#include "MarbleDirs.h"

#include "PlaceMark.h"

PlaceMark::PlaceMark()
  : m_popularity( 0 ),
    m_symbolIndex( 0 ),
    m_popularityIndex( 0 )
{
}

PlaceMark::PlaceMark( const QString& name )
  : KMLFeature( name ),
    m_popularity( 0 ),
    m_symbolIndex( 0 ),
    m_popularityIndex( 0 )
{
}

GeoPoint PlaceMark::coordinate() const
{
    return m_coordinate;
}

void PlaceMark::coordinate( double& lon, double& lat )
{
    m_coordinate.geoCoordinates( lon, lat );
}

void PlaceMark::setCoordinate( double lon, double lat )
{
    m_coordinate = GeoPoint( lon, lat );
}

const QChar PlaceMark::role() const
{
    return m_role;
}

void PlaceMark::setRole( const QChar &role )
{
    m_role = role;
}

const QString PlaceMark::countryCode() const
{
    return m_countrycode;
}

void PlaceMark::setCountryCode( const QString &countrycode )
{
    m_countrycode = countrycode;
}

const int PlaceMark::symbolIndex() const
{
    return m_symbolIndex;
}

void PlaceMark::setSymbolIndex( int index )
{
    m_symbolIndex = index;
}

const int PlaceMark::popularityIndex() const
{
    return m_popularityIndex;
}

void PlaceMark::setPopularityIndex( int popularityIndex )
{
    m_popularityIndex = popularityIndex;
}

const int PlaceMark::popularity() const
{
    return m_popularity;
}

void PlaceMark::setPopularity( int popularity )
{
    m_popularity = popularity;
}

const QSize PlaceMark::symbolSize() const
{

    static QSize placeSize[ 23 ] = {
        QSize( 5, 5 ),
        QSize( 5, 5 ),
        QSize( 5, 5 ),
        QSize( 5, 5 ),
        
        QSize( 5, 5 ),
        QSize( 5, 5 ),
        QSize( 5, 5 ),
        QSize( 5, 5 ),
        
        QSize( 8, 8 ),
        QSize( 8, 8 ),
        QSize( 8, 8 ),
        QSize( 8, 8 ),
        
        QSize( 8, 8 ),
        QSize( 8, 8 ),
        QSize( 8, 8 ),
        QSize( 8, 8 ),
        
        QSize( 8, 8 ),
        QSize( 8, 8 ),
        QSize( 5, 5 ),
        QSize( 6, 6 ),

        QSize( 12, 12 ),
        QSize( 12, 12 ),
        QSize( 7, 7 )
    };

    return placeSize[ m_symbolIndex ];
}


const QPixmap PlaceMark::symbolPixmap() const
{
    static QPixmap placeSymbol[ 23 ] = {
        QPixmap( MarbleDirs::path( "bitmaps/city_4_white.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_4_yellow.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_4_orange.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_4_red.png" ) ),
        
        QPixmap( MarbleDirs::path( "bitmaps/city_3_white.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_3_yellow.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_3_orange.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_3_red.png" ) ),
        
        QPixmap( MarbleDirs::path( "bitmaps/city_2_white.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_2_yellow.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_2_orange.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_2_red.png" ) ),
        
        QPixmap( MarbleDirs::path( "bitmaps/city_1_white.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_1_yellow.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_1_orange.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/city_1_red.png" ) ),
        
        QPixmap( MarbleDirs::path( "bitmaps/mountain_1.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/volcano_1.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/pole_1.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/pole_2.png" ) ),

        QPixmap( MarbleDirs::path( "bitmaps/shipwreck.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/airport.png" ) ),
        QPixmap( MarbleDirs::path( "bitmaps/default_location.png" ) )
    };

    return placeSymbol[ m_symbolIndex ];
}




void PlaceMark::pack( QDataStream& stream ) const
{
    KMLFeature::pack( stream );

    stream << m_popularity;
    stream << m_symbolIndex;
    stream << m_popularityIndex;
    stream << m_role;
    stream << m_countrycode;

    /*
     * pack coordinates
     */
    double longitude;
    double latitude;

    m_coordinate.geoCoordinates( longitude, latitude );
    stream << longitude;
    stream << latitude;
}


void PlaceMark::unpack( QDataStream& stream )
{
    KMLFeature::unpack( stream );

    stream >> m_popularity;
    stream >> m_symbolIndex;
    stream >> m_popularityIndex;
    stream >> m_role;
    stream >> m_countrycode;

    /*
     * unpack coordinates
     */
    double longitude;
    double latitude;

    stream >> longitude;
    stream >> latitude;
    setCoordinate( longitude, latitude );
}
