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
#include "GeoDataStyle.h"

bool GeoDataPlaceMark::s_defaultStyleInitialized = false;

QPixmap* GeoDataPlaceMark::s_placeSymbol[];

QSize  GeoDataPlaceMark::s_placeSize[];

QFont  GeoDataPlaceMark::s_placeLabelFont[];

QColor GeoDataPlaceMark::s_placeLabelColor[];


GeoDataPlaceMark::GeoDataPlaceMark()
  : m_popularity( 0 ),
    m_style( 0 ),
    m_symbolIndex( Unknown ),
    m_popularityIndex( 0 )
{
}

GeoDataPlaceMark::GeoDataPlaceMark( const QString& name )
  : GeoDataFeature( name ),
    m_popularity( 0 ),
    m_symbolIndex( Unknown ),
    m_popularityIndex( 0 )
{
}

void GeoDataPlaceMark::initializeDefaultStyle()
{
    // We need to do this similar to the way KCmdLineOptions works in the future:
    // Having a PlaceMarkStyleProperty properties[] would help here greatly.

    QString defaultFamily = "Sans Serif";

#ifdef Q_OS_MACX
    int defaultSize = 10;
#else
    int defaultSize = 8;
#endif

    s_placeSymbol[Empty]
        = new QPixmap();
    s_placeSize[Empty]                = QSize( 0, 0 );
    s_placeLabelFont[Empty]           = QFont( defaultFamily,  8, 50, false );
    s_placeLabelColor[Empty]          = QColor( Qt::black );

    s_placeSymbol[Default]
        = new QPixmap( MarbleDirs::path( "bitmaps/default_location.png" ) );
    s_placeSize[Default]              = QSize( 0, 0 );
    s_placeLabelFont[Default]         = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[Default]        = QColor( Qt::black );

    s_placeSymbol[Unknown]
        = new QPixmap();
    s_placeSize[Unknown]              = QSize( 0, 0 );
    s_placeLabelFont[Unknown]         = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[Unknown]        = QColor( Qt::black );

    s_placeSymbol[SmallCity]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_4_white.png" ) );
    s_placeSize[SmallCity]            = QSize( 5, 5 );
    s_placeLabelFont[SmallCity]       = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[SmallCity]      = QColor( Qt::black );

    s_placeSymbol[SmallCountyCapital]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_4_yellow.png" ) );
    s_placeSize[SmallCountyCapital]      = QSize( 5, 5 );
    s_placeLabelFont[SmallCountyCapital] = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[SmallCountyCapital]= QColor( Qt::black );

    s_placeSymbol[SmallStateCapital]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_4_orange.png" ) );
    s_placeSize[SmallStateCapital]       = QSize( 5, 5 );
    s_placeLabelFont[SmallStateCapital]  = QFont( defaultFamily, defaultSize, 50, true );
    s_placeLabelColor[SmallStateCapital] = QColor( Qt::black );

    s_placeSymbol[SmallNationCapital]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_4_red.png" ) );
    s_placeSize[SmallNationCapital]   = QSize( 5, 5 );
    s_placeLabelFont[SmallNationCapital]  = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[SmallNationCapital] = QColor( Qt::black );

    s_placeSymbol[MediumCity]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_3_white.png" ) );
    s_placeSize[MediumCity]           = QSize( 5, 5 );
    s_placeLabelFont[MediumCity]       = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[MediumCity]      = QColor( Qt::black );

    s_placeSymbol[MediumCountyCapital]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_3_yellow.png" ) );
    s_placeSize[MediumCountyCapital]  = QSize( 5, 5 );
    s_placeLabelFont[MediumCountyCapital] = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[MediumCountyCapital]= QColor( Qt::black );

    s_placeSymbol[MediumStateCapital] 
        = new QPixmap( MarbleDirs::path( "bitmaps/city_3_orange.png" ) );
    s_placeSize[MediumStateCapital]   = QSize( 5, 5 );
    s_placeLabelFont[MediumStateCapital]  = QFont( defaultFamily, defaultSize, 50, true );
    s_placeLabelColor[MediumStateCapital] = QColor( Qt::black );

    s_placeSymbol[MediumNationCapital]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_3_red.png" ) );
    s_placeSize[MediumNationCapital]  = QSize( 5, 5 );
    s_placeLabelFont[MediumNationCapital]  = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[MediumNationCapital] = QColor( Qt::black );

    s_placeSymbol[BigCity]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_2_white.png" ) );
    s_placeSize[BigCity]              = QSize( 8, 8 );
    s_placeLabelFont[BigCity]       = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[BigCity]      = QColor( Qt::black );

    s_placeSymbol[BigCountyCapital]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_2_yellow.png" ) );
    s_placeSize[BigCountyCapital]     = QSize( 8, 8 );
    s_placeLabelFont[BigCountyCapital] = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[BigCountyCapital]= QColor( Qt::black );

    s_placeSymbol[BigStateCapital] 
        = new QPixmap( MarbleDirs::path( "bitmaps/city_2_orange.png" ) );
    s_placeSize[BigStateCapital]      = QSize( 8, 8 );
    s_placeLabelFont[BigStateCapital]  = QFont( defaultFamily, defaultSize, 50, true );
    s_placeLabelColor[BigStateCapital] = QColor( Qt::black );

    s_placeSymbol[BigNationCapital]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_2_red.png" ) );
    s_placeSize[BigNationCapital]     = QSize( 8, 8 );
    s_placeLabelFont[BigNationCapital]  = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[BigNationCapital] = QColor( Qt::black );

    s_placeSymbol[LargeCity]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_1_white.png" ) );
    s_placeSize[LargeCity]            = QSize( 8, 8 );
    s_placeLabelFont[LargeCity]       = QFont( defaultFamily, defaultSize, 75, false );
    s_placeLabelColor[LargeCity]      = QColor( Qt::black );

    s_placeSymbol[LargeCountyCapital]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_1_yellow.png" ) );
    s_placeSize[LargeCountyCapital]   = QSize( 8, 8 );
    s_placeLabelFont[LargeCountyCapital] = QFont( defaultFamily, defaultSize, 75, false );
    s_placeLabelColor[LargeCountyCapital]= QColor( Qt::black );

    s_placeSymbol[LargeStateCapital] 
        = new QPixmap( MarbleDirs::path( "bitmaps/city_1_orange.png" ) );
    s_placeSize[LargeStateCapital]    = QSize( 8, 8 );
    s_placeLabelFont[LargeStateCapital]  = QFont( defaultFamily, defaultSize, 75, true );
    s_placeLabelColor[LargeStateCapital] = QColor( Qt::black );

    s_placeSymbol[LargeNationCapital]
        = new QPixmap( MarbleDirs::path( "bitmaps/city_1_red.png" ) );
    s_placeSize[LargeNationCapital]   = QSize( 8, 8 );
    s_placeLabelFont[LargeNationCapital]  = QFont( defaultFamily, defaultSize, 75, false );
    s_placeLabelColor[LargeNationCapital] = QColor( Qt::black );

    s_placeSymbol[Mountain]
        = new QPixmap( MarbleDirs::path( "bitmaps/mountain_1.png" ) );
    s_placeSize[Mountain]             = QSize( 8, 8 );
    s_placeLabelFont[Mountain]        = QFont( defaultFamily, (int)(0.875 * defaultSize), 50, false );
    s_placeLabelColor[Mountain]       = QColor( Qt::black );

    s_placeSymbol[Volcano]
        = new QPixmap( MarbleDirs::path( "bitmaps/volcano_1.png" ) );
    s_placeSize[Volcano]              = QSize( 8, 8 );
    s_placeLabelFont[Volcano]         = QFont( defaultFamily, (int)(0.875 * defaultSize), 50, false );
    s_placeLabelColor[Volcano]        = QColor( Qt::black );

    s_placeSymbol[GeographicPole]
        = new QPixmap( MarbleDirs::path( "bitmaps/pole_1.png" ) );
    s_placeSize[GeographicPole]       = QSize( 5, 5 );
    s_placeLabelFont[GeographicPole]  = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[GeographicPole] = QColor( Qt::black );

    s_placeSymbol[MagneticPole]
        = new QPixmap( MarbleDirs::path( "bitmaps/pole_2.png" ) );
    s_placeSize[MagneticPole]         = QSize( 6, 6 );
    s_placeLabelFont[MagneticPole]    = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[MagneticPole]   = QColor( Qt::black );

    s_placeSymbol[ShipWreck]
        = new QPixmap( MarbleDirs::path( "bitmaps/shipwreck.png" ) );
    s_placeSize[ShipWreck]            = QSize( 12, 12 );
    s_placeLabelFont[ShipWreck]       = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[ShipWreck]      = QColor( Qt::black );

    s_placeSymbol[AirPort]
        = new QPixmap( MarbleDirs::path( "bitmaps/airport.png" ) );
    s_placeSize[AirPort]              = QSize( 12, 12 );
    s_placeLabelFont[AirPort]         = QFont( defaultFamily, defaultSize, 50, false );
    s_placeLabelColor[AirPort]        = QColor( Qt::black );


    s_placeLabelFont[SmallNationCapital].setUnderline(true);
    s_placeLabelFont[MediumNationCapital].setUnderline(true);
    s_placeLabelFont[BigNationCapital].setUnderline(true);
    s_placeLabelFont[LargeNationCapital].setUnderline(true);


    s_defaultStyleInitialized = true;
}

GeoPoint GeoDataPlaceMark::coordinate() const
{
    return m_coordinate;
}

void GeoDataPlaceMark::coordinate( double& lon, double& lat )
{
    m_coordinate.geoCoordinates( lon, lat );
}

void GeoDataPlaceMark::setCoordinate( double lon, double lat )
{
    m_coordinate = GeoPoint( lon, lat );
}

const QChar GeoDataPlaceMark::role() const
{
    return m_role;
}

void GeoDataPlaceMark::setRole( const QChar &role )
{
    m_role = role;
}

const QString GeoDataPlaceMark::countryCode() const
{
    return m_countrycode;
}

void GeoDataPlaceMark::setCountryCode( const QString &countrycode )
{
    m_countrycode = countrycode;
}

const GeoDataPlaceMark::SymbolIndex GeoDataPlaceMark::symbolIndex() const
{
    return m_symbolIndex;
}

void GeoDataPlaceMark::setSymbolIndex( GeoDataPlaceMark::SymbolIndex index )
{
    m_symbolIndex = index;
}

const int GeoDataPlaceMark::popularityIndex() const
{
    return m_popularityIndex;
}

void GeoDataPlaceMark::setPopularityIndex( int popularityIndex )
{
    m_popularityIndex = popularityIndex;
}

const qint64 GeoDataPlaceMark::popularity() const
{
    return m_popularity;
}

void GeoDataPlaceMark::setPopularity( qint64 popularity )
{
    m_popularity = popularity;
}

const QSize GeoDataPlaceMark::symbolSize() const
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyle();

    return s_placeSize[ m_symbolIndex ];
}

const QPixmap GeoDataPlaceMark::symbolPixmap() const
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyle();

    return *s_placeSymbol[ m_symbolIndex ];
}

const QFont GeoDataPlaceMark::labelFont( int id )
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyle();

    return s_placeLabelFont[ id ];
}

const QColor GeoDataPlaceMark::labelColor( int id )
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyle();

    return s_placeLabelColor[ id ];
}

void GeoDataPlaceMark::resetDefaultStyle()
{
    s_defaultStyleInitialized = false;
}

void GeoDataPlaceMark::pack( QDataStream& stream ) const
{
    GeoDataFeature::pack( stream );

    stream << m_popularity;
    stream << (int)( m_symbolIndex );
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


void GeoDataPlaceMark::unpack( QDataStream& stream )
{
    GeoDataFeature::unpack( stream );

    int tmpSymbolIndex = 0;

    stream >> m_popularity;
    stream >> tmpSymbolIndex;
    m_symbolIndex = ( GeoDataPlaceMark::SymbolIndex ) tmpSymbolIndex;
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
