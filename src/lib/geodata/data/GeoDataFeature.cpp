//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataFeature.h"

#include <QtCore/QDebug>
#include <QtCore/QDataStream>
#include <QtCore/QSize>
#include <QtGui/QPixmap>

#include "MarbleDirs.h"

#include "GeoDataStyle.h"

bool GeoDataFeature::s_defaultStyleInitialized = false;

GeoDataStyle* GeoDataFeature::s_defaultStyle[];


GeoDataFeature::GeoDataFeature() :
    m_visualCategory( Unknown ),
    m_popularity( 0 ),
    m_popularityIndex( 0 ),
    m_visible( true ),
    m_role(' '),
    m_style( 0 )
{
}

GeoDataFeature::GeoDataFeature( const QString& name ) :
    m_visualCategory( Unknown ),
    m_name( name ),
    m_popularity( 0 ),
    m_popularityIndex( 0 ),
    m_visible( true ),
    m_role(' '),
    m_style( 0 )
{
}

GeoDataFeature::~GeoDataFeature()
{
}

void GeoDataFeature::initializeDefaultStyles()
{
    // We need to do this similar to the way KCmdLineOptions works in
    // the future: Having a PlaceMarkStyleProperty properties[] would
    // help here greatly.

    QString defaultFamily = "Sans Serif";

#ifdef Q_OS_MACX
    int defaultSize = 10;
#else
    int defaultSize = 8;
#endif

    s_defaultStyle[None]
        = new GeoDataStyle( QPixmap(), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Default]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/default_location.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Unknown]
        = new GeoDataStyle( QPixmap(), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[SmallCity]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_4_white.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[SmallCountyCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_4_yellow.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[SmallStateCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_4_orange.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, true  ), QColor( Qt::black ) );

    s_defaultStyle[SmallNationCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_4_red.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[MediumCity]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_3_white.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[MediumCountyCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_3_yellow.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[MediumStateCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_3_orange.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, true  ), QColor( Qt::black ) );

    s_defaultStyle[MediumNationCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_3_red.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[BigCity]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_2_white.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[BigCountyCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_2_yellow.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[BigStateCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_2_orange.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, true  ), QColor( Qt::black ) );

    s_defaultStyle[BigNationCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_2_red.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[LargeCity]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_1_white.png" ) ), 
              QFont( defaultFamily, defaultSize, 75, false ), QColor( Qt::black ) );

    s_defaultStyle[LargeCountyCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_1_yellow.png" ) ), 
              QFont( defaultFamily, defaultSize, 75, false ), QColor( Qt::black ) );

    s_defaultStyle[LargeStateCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_1_orange.png" ) ), 
              QFont( defaultFamily, defaultSize, 75, true  ), QColor( Qt::black ) );

    s_defaultStyle[LargeNationCapital]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/city_1_red.png" ) ), 
              QFont( defaultFamily, defaultSize, 75, false ), QColor( Qt::black ) );

    s_defaultStyle[Nation]
        = new GeoDataStyle( QPixmap(), 
              QFont( defaultFamily, (int)(defaultSize * 1.2 ), 75, false ), QColor( "#404040" ) );
    // Align area labels centered
    s_defaultStyle[Nation] -> labelStyle() -> setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[Mountain]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/mountain_1.png" ) ), 
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Volcano]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/volcano_1.png" ) ), 
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Continent]
        = new GeoDataStyle( QPixmap(), 
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) );
    // Align area labels centered
    s_defaultStyle[Continent] -> labelStyle() -> setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[Ocean]
        = new GeoDataStyle( QPixmap(), 
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, true ), QColor( "#2c72c7" ) );
    // Align area labels centered
    s_defaultStyle[Ocean] -> labelStyle() -> setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeographicPole]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/pole_1.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[MagneticPole]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/pole_2.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[ShipWreck]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/shipwreck.png" ) ), 
              QFont( defaultFamily, (int)(defaultSize * 0.8 ), 50, false ), QColor( Qt::black ) );

    s_defaultStyle[AirPort]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/airport.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyleInitialized = true;

    QFont tmp;

/*
    // Fonts for areas ...
    // This will only work with Qt 4.4
    tmp = s_defaultStyle[Continent] -> labelStyle()-> font();
    tmp.setLetterSpacing( 2 );
    tmp.setSmallCaps( true );
    s_defaultStyle[Continent] -> labelStyle()-> setFont( tmp );
*/

    // Now we need to underline the capitals ...

    tmp = s_defaultStyle[SmallNationCapital] -> labelStyle()-> font();
    tmp.setUnderline( true );
    s_defaultStyle[SmallNationCapital] -> labelStyle()-> setFont( tmp );

    tmp = s_defaultStyle[MediumNationCapital] -> labelStyle()-> font();
    tmp.setUnderline( true );
    s_defaultStyle[MediumNationCapital] -> labelStyle()-> setFont( tmp );

    tmp = s_defaultStyle[BigNationCapital] -> labelStyle()-> font();
    tmp.setUnderline( true );
    s_defaultStyle[BigNationCapital] -> labelStyle()-> setFont( tmp );

    tmp = s_defaultStyle[LargeNationCapital] -> labelStyle()-> font();
    tmp.setUnderline( true );
    s_defaultStyle[LargeNationCapital] -> labelStyle()-> setFont( tmp );
}

QString GeoDataFeature::name() const
{
    return m_name;
}

void GeoDataFeature::setName( const QString &value )
{
    m_name = value;
}

QString GeoDataFeature::address() const
{
    return m_address;
}

void GeoDataFeature::setAddress( const QString &value)
{
    m_address = value;
}

QString GeoDataFeature::phoneNumber() const
{
    return m_phoneNumber;
}

void GeoDataFeature::setPhoneNumber( const QString &value)
{
    m_phoneNumber = value;
}

QString GeoDataFeature::description() const
{
    return m_description;
}

void GeoDataFeature::setDescription( const QString &value)
{
    m_description = value;
}

bool GeoDataFeature::isVisible() const
{
    return m_visible;
}

void GeoDataFeature::setVisible( bool value )
{
    m_visible = value;
}

GeoDataStyle* GeoDataFeature::style() const
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyles();

    if ( m_visualCategory != None )
    {
        return s_defaultStyle[ m_visualCategory ];
    }
    else
    {
        if ( m_style != 0 )
            return static_cast<GeoDataStyle*>(m_style);
        else
        {
            // This should not happen
            qDebug() << "No Style got assigned!";
            return new GeoDataStyle( QPixmap(), 
              QFont( "Sans Serif",  8, 50, false ), QColor( Qt::black ) );
        }
    }
}

void GeoDataFeature::setStyle( GeoDataStyle* style )
{
    m_style = style;
}

const GeoDataFeature::GeoDataVisualCategory GeoDataFeature::visualCategory() const
{
    return m_visualCategory;
}

void GeoDataFeature::setVisualCategory( GeoDataFeature::GeoDataVisualCategory index )
{
    m_visualCategory = index;
}

const QChar GeoDataFeature::role() const
{
    return m_role;
}

void GeoDataFeature::setRole( const QChar &role )
{
    m_role = role;
}

const int GeoDataFeature::popularityIndex() const
{
    return m_popularityIndex;
}

void GeoDataFeature::setPopularityIndex( int popularityIndex )
{
    m_popularityIndex = popularityIndex;
}

const qint64 GeoDataFeature::popularity() const
{
    return m_popularity;
}

void GeoDataFeature::setPopularity( qint64 popularity )
{
    m_popularity = popularity;
}

const QSize GeoDataFeature::symbolSize() const
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyles();

    return style()->iconStyle()->icon().size();
}

const QPixmap GeoDataFeature::symbolPixmap() const
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyles();

    return style()->iconStyle()->icon();
}

void GeoDataFeature::resetDefaultStyles()
{
    s_defaultStyleInitialized = false;
}

void GeoDataFeature::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << m_name;
    stream << m_address;
    stream << m_phoneNumber;
    stream << m_description;
    stream << m_visible;
    stream << m_role;
}

void GeoDataFeature::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> m_name;
    stream >> m_address;
    stream >> m_phoneNumber;
    stream >> m_description;
    stream >> m_visible;
    stream >> m_role;
}
