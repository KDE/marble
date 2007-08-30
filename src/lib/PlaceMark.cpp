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


#include "PlaceMark.h"

#include <cmath>

#include <QtCore/QDebug>

#include "GeoPoint.h"
#include "MarbleDirs.h"



PlaceMark::PlaceMark()
  : m_coordinate()
{
    m_symbol       = 0;
    m_population   = 0;
    m_popidx       = 0;
    //m_symbolPixmap = QPixmap();
    //m_labelPixmap  = QPixmap();

    // Bounding box for the label
    // FIXME: Should be moved to the view.
    //m_textRect     = QRect();
    m_selected     = 0; // 0: not selected 1: centered 2:hover
}

PlaceMark::PlaceMark( const QString& name )
  : KMLFeature( name ),
    m_coordinate()
{
    m_symbol       = 0;
    //m_labelPixmap  = QPixmap();
}

void PlaceMark::coordinate( double& lon, double& lat )
{
    m_coordinate.geoCoordinates( lon, lat );
}

void PlaceMark::setCoordinate( double lon, double lat )
{
    m_coordinate = GeoPoint( lon, lat );
}


const QSize PlaceMark::symbolSize() const
{

    static QSize placesize[22] = {
	QSize(5,5),
	QSize(5,5),
	QSize(5,5),
	QSize(5,5),

	QSize(5,5),
	QSize(5,5),
	QSize(5,5),
	QSize(5,5),

	QSize(8,8),
	QSize(8,8),
	QSize(8,8),
	QSize(8,8),

	QSize(8,8),
	QSize(8,8),
	QSize(8,8),
	QSize(8,8),

	QSize(5,5),
	QSize(6,6),
	QSize(8,8),
	QSize(8,8),

    QSize(12,12),
    QSize(7,7)
    };

    return placesize[m_symbol];
}


const QPixmap PlaceMark::symbolPixmap() const
{

    static QPixmap placesymbol[22] = {
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

	QPixmap( MarbleDirs::path( "bitmaps/pole_1.png" ) ),
	QPixmap( MarbleDirs::path( "bitmaps/pole_2.png" ) ),
	QPixmap( MarbleDirs::path( "bitmaps/mountain_1.png" ) ),
	QPixmap( MarbleDirs::path( "bitmaps/volcano_1.png" ) ),

    QPixmap( MarbleDirs::path( "bitmaps/airport.png" ) ),
    QPixmap( MarbleDirs::path( "bitmaps/default_location.png" ) )
    };

    return placesymbol[m_symbol];
}


void PlaceMark::pack( QDataStream& stream ) const
{
    KMLFeature::pack( stream );

    stream << m_population;
    stream << m_symbol;
    stream << m_popidx;
    stream << m_role;
    stream << m_countrycode;

    /*
     * pack coordinates
     */
    double lon;
    double lat;

    m_coordinate.geoCoordinates( lon, lat );
    stream << lon;
    stream << lat;
}


void PlaceMark::unpack( QDataStream& stream )
{
    KMLFeature::unpack( stream );

    stream >> m_population;
    stream >> m_symbol;
    stream >> m_popidx;
    stream >> m_role;
    stream >> m_countrycode;

    /*
     * unpack coordinates
     */
    double lon;
    double lat;

    stream >> lon;
    stream >> lat;
    setCoordinate( lon, lat );
}
