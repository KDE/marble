//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataFeature.h"

#include <QtCore/QDebug>
#include <QtCore/QDataStream>
#include <QtCore/QSize>
#include <QtGui/QPixmap>

#include "MarbleDirs.h"

#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"

#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"

#include "GeoDataFeature_p.h"

namespace Marble {
QFont GeoDataFeature::s_defaultFont = QFont("Sans Serif");

bool GeoDataFeature::s_defaultStyleInitialized = false;
GeoDataStyle* GeoDataFeature::s_defaultStyle[GeoDataFeature::LastIndex];


GeoDataFeature::GeoDataFeature()
    :d( new GeoDataFeaturePrivate() )
{
    p()->ref.ref();
}

GeoDataFeature::GeoDataFeature( const GeoDataFeature& other )
    : GeoDataObject( other ),
      d( other.d )
{
    p()->ref.ref();
}

GeoDataFeature::GeoDataFeature( const GeoDataContainer& other )
    : GeoDataObject( other ),
      d( other.GeoDataFeature::d )
{
    p()->ref.ref();
}

GeoDataFeature::GeoDataFeature( const GeoDataFolder& other )
    : GeoDataObject( other ),
      d( other.GeoDataFeature::d )
{
    p()->ref.ref();
}

GeoDataFeature::GeoDataFeature( const GeoDataDocument& other )
    : GeoDataObject( other ),
      d( other.GeoDataFeature::d )
{
    p()->ref.ref();
}

GeoDataFeature::GeoDataFeature( const GeoDataPlacemark& other )
    : GeoDataObject( other ),
      d( other.GeoDataFeature::d )
{
    p()->ref.ref();
}

GeoDataFeature::GeoDataFeature( const QString& name )
    :d( new GeoDataFeaturePrivate() )
{
    p()->ref.ref();
    p()->m_name = name;
}

GeoDataFeature::GeoDataFeature( GeoDataFeaturePrivate *priv ) : d( priv )
{
    p()->ref.ref();
}
    
GeoDataFeature::~GeoDataFeature()
{
    if (!p()->ref.deref())
        delete d;
}

GeoDataFeaturePrivate* GeoDataFeature::p() const
{
    return static_cast<GeoDataFeaturePrivate*>(d);
}

GeoDataFeature& GeoDataFeature::operator=( const GeoDataFeature& other )
{
    GeoDataObject::operator=( other );
    if (!p()->ref.deref())
        delete d;

    d = other.d;
    p()->ref.ref();
    
    return *this;
}

void GeoDataFeature::initializeDefaultStyles()
{
    // We need to do this similar to the way KCmdLineOptions works in
    // the future: Having a PlaceMarkStyleProperty properties[] would
    // help here greatly.

    QString defaultFamily = s_defaultFont.family();

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
    s_defaultStyle[Nation]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[Mountain]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/mountain_1.png" ) ), 
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Volcano]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/volcano_1.png" ) ), 
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Mons]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/mountain_1.png" ) ), 
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Valley]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/valley.png" ) ), 
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Continent]
        = new GeoDataStyle( QPixmap(), 
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) );
    // Align area labels centered
    s_defaultStyle[Continent]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[Ocean]
        = new GeoDataStyle( QPixmap(), 
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, true ), QColor( "#2c72c7" ) );
    // Align area labels centered
    s_defaultStyle[Ocean]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[OtherTerrain]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/other.png" ) ), 
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Crater]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/crater.png" ) ), 
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Mare]
        = new GeoDataStyle( QPixmap(), 
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) );
    // Align area labels centered
    s_defaultStyle[Mare]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

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

    s_defaultStyle[Wikipedia]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/wikipedia.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[OsmSite]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/osm.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[Coordinate]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/coordinate.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );


    s_defaultStyle[MannedLandingSite]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/manned_landing.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[RoboticRover]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/robotic_rover.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[UnmannedSoftLandingSite]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/unmanned_soft_landing.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyle[UnmannedHardLandingSite]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/unmanned_hard_landing.png" ) ), 
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::black ) );

    s_defaultStyleInitialized = true;
    s_defaultFont = QFont("Sans Serif");

    QFont tmp;


#if QT_VERSION >= 0x040400
    // Fonts for areas ...
    tmp = s_defaultStyle[Continent]->labelStyle().font();
    tmp.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    tmp.setCapitalization( QFont::SmallCaps );
    tmp.setBold( true );
    s_defaultStyle[Continent]->labelStyle().setFont( tmp );

    // Fonts for areas ...
    tmp = s_defaultStyle[Mare]->labelStyle().font();
    tmp.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    tmp.setCapitalization( QFont::SmallCaps );
    tmp.setBold( true );
    s_defaultStyle[Mare]->labelStyle().setFont( tmp );
#endif

    // Now we need to underline the capitals ...

    tmp = s_defaultStyle[SmallNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    s_defaultStyle[SmallNationCapital]->labelStyle().setFont( tmp );

    tmp = s_defaultStyle[MediumNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    s_defaultStyle[MediumNationCapital]->labelStyle().setFont( tmp );

    tmp = s_defaultStyle[BigNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    s_defaultStyle[BigNationCapital]->labelStyle().setFont( tmp );

    tmp = s_defaultStyle[LargeNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    s_defaultStyle[LargeNationCapital]->labelStyle().setFont( tmp );
}

QFont GeoDataFeature::defaultFont()
{
    return s_defaultFont;
}

void GeoDataFeature::setDefaultFont( const QFont& font )
{
    s_defaultFont = font;
    s_defaultStyleInitialized = false;
}

QString GeoDataFeature::name() const
{
    return p()->m_name;
}

void GeoDataFeature::setName( const QString &value )
{
    p()->m_name = value;
}

QString GeoDataFeature::address() const
{
    return p()->m_address;
}

void GeoDataFeature::setAddress( const QString &value)
{
    p()->m_address = value;
}

QString GeoDataFeature::phoneNumber() const
{
    return p()->m_phoneNumber;
}

void GeoDataFeature::setPhoneNumber( const QString &value)
{
    p()->m_phoneNumber = value;
}

QString GeoDataFeature::description() const
{
    return p()->m_description;
}

void GeoDataFeature::setDescription( const QString &value)
{
    p()->m_description = value;
}

QString GeoDataFeature::styleUrl() const
{
    return p()->m_styleUrl;
}

void GeoDataFeature::setStyleUrl( const QString &value)
{
    p()->m_styleUrl = value;
}

bool GeoDataFeature::isVisible() const
{
    return p()->m_visible;
}

void GeoDataFeature::setVisible( bool value )
{
    p()->m_visible = value;
}

GeoDataStyle* GeoDataFeature::style() const
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyles();

    if ( p()->m_visualCategory != None )
    {
        return s_defaultStyle[ p()->m_visualCategory ];
    }
    else
    {
        if ( p()->m_style != 0 ) {
            return p()->m_style;
        } else
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
    p()->m_style = style;
}

GeoDataFeature::GeoDataVisualCategory GeoDataFeature::visualCategory() const
{
    return p()->m_visualCategory;
}

void GeoDataFeature::setVisualCategory( GeoDataFeature::GeoDataVisualCategory index )
{
    p()->m_visualCategory = index;
}

const QChar GeoDataFeature::role() const
{
    return p()->m_role;
}

void GeoDataFeature::setRole( const QChar &role )
{
    p()->m_role = role;
}

GeoDataStyleMap* GeoDataFeature::styleMap() const
{
    return p()->m_styleMap;
}

void GeoDataFeature::setStyleMap( GeoDataStyleMap* styleMap )
{
    p()->m_styleMap = styleMap;
}

int GeoDataFeature::popularityIndex() const
{
    return p()->m_popularityIndex;
}

void GeoDataFeature::setPopularityIndex( int popularityIndex )
{
    p()->m_popularityIndex = popularityIndex;
}

qint64 GeoDataFeature::popularity() const
{
    return p()->m_popularity;
}

void GeoDataFeature::setPopularity( qint64 popularity )
{
    p()->m_popularity = popularity;
}

const QSize GeoDataFeature::symbolSize() const
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyles();

    return style()->iconStyle().icon().size();
}

const QPixmap GeoDataFeature::symbolPixmap() const
{
    if ( s_defaultStyleInitialized == false )
        initializeDefaultStyles();

    return style()->iconStyle().icon();
}

void GeoDataFeature::resetDefaultStyles()
{
    s_defaultStyleInitialized = false;
}

void GeoDataFeature::detach()
{
    if(p()->ref == 1)
        return;

    GeoDataFeaturePrivate* new_d = static_cast<GeoDataFeaturePrivate*>(p()->copy());

    if (!p()->ref.deref())
        delete d;

    d = new_d;
}

void GeoDataFeature::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << p()->m_name;
    stream << p()->m_address;
    stream << p()->m_phoneNumber;
    stream << p()->m_description;
    stream << p()->m_visible;
//    stream << p()->m_visualCategory;
    stream << p()->m_role;
    stream << p()->m_popularity;
    stream << p()->m_popularityIndex;
}

void GeoDataFeature::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> p()->m_name;
    stream >> p()->m_address;
    stream >> p()->m_phoneNumber;
    stream >> p()->m_description;
    stream >> p()->m_visible;
//    stream >> (int)p()->m_visualCategory;
    stream >> p()->m_role;
    stream >> p()->m_popularity;
    stream >> p()->m_popularityIndex;
}

}
