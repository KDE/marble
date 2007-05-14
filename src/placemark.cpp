//
// C++ Implementation: gpplacemark
//
// Description: 
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "placemark.h"

#include <cmath>

#include <QtCore/QDebug>

#include "GeoPoint.h"
#include "katlasdirs.h"



PlaceMark::PlaceMark()
  : m_coordinate(),
    m_name()
{
    m_symbol       = 0;
    m_population   = 0;
    m_popidx       = 0;
    m_symbolPixmap = QPixmap();
    m_labelPixmap  = QPixmap();

    // Bounding box for the label
    // FIXME: Should be moved to the view.
    m_rect         = QRect();
    m_selected     = 0; // 0: not selected 1: centered 2:hover
}

PlaceMark::PlaceMark( const QString& _name )
  : m_coordinate(),
    m_name( _name )
{
    m_symbol       = 0;
    m_symbolPixmap = QPixmap();
    m_labelPixmap  = QPixmap();
}


void PlaceMark::coordinate( float& lon, float& lat )
{
    m_coordinate.geoCoordinates( lon, lat );
}

void PlaceMark::setCoordinate( float lon, float lat )
{
    m_coordinate = GeoPoint( lon, lat );
}


const QSize PlaceMark::symbolSize() const
{

    static QSize placesize[20] = {
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
	QSize(6,6),
	QSize(6,6),
    };

    return placesize[m_symbol];
}


const QPixmap PlaceMark::symbolPixmap() const
{

    static QPixmap placesymbol[20] = {
	QPixmap( KAtlasDirs::path( "bitmaps/city_4_white.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_4_yellow.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_4_orange.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_4_red.png" ) ),	

	QPixmap( KAtlasDirs::path( "bitmaps/city_3_white.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_3_yellow.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_3_orange.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_3_red.png" ) ),

	QPixmap( KAtlasDirs::path( "bitmaps/city_2_white.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_2_yellow.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_2_orange.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_2_red.png" ) ),

	QPixmap( KAtlasDirs::path( "bitmaps/city_1_white.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_1_yellow.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_1_orange.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/city_1_red.png" ) ),

	QPixmap( KAtlasDirs::path( "bitmaps/pole_1.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/pole_2.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/mountain_1.png" ) ),
	QPixmap( KAtlasDirs::path( "bitmaps/volcano_1.png" ) )
    };

    return placesymbol[m_symbol];
}
