//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataFeature.h"
#include "GeoDataFeature_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QSize>
#include <QtGui/QPixmap>

#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"

#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataRegion.h"


namespace Marble
{
QFont GeoDataFeature::s_defaultFont = QFont("Sans Serif");

bool GeoDataFeature::s_defaultStyleInitialized = false;
GeoDataStyle* GeoDataFeature::s_defaultStyle[GeoDataFeature::LastIndex];


GeoDataFeature::GeoDataFeature()
    :d( new GeoDataFeaturePrivate() )
{
    d->ref.ref();
}

GeoDataFeature::GeoDataFeature( const GeoDataFeature& other )
    : GeoDataObject( other ),
      d( other.d )
{
    d->ref.ref();
}

GeoDataFeature::GeoDataFeature( const QString& name )
    : d( new GeoDataFeaturePrivate() )
{
    d->ref.ref();
    d->m_name = name;
}

GeoDataFeature::GeoDataFeature( GeoDataFeaturePrivate *priv ) 
    : d( priv )
{
    d->ref.ref();
}
    
GeoDataFeature::~GeoDataFeature()
{
    if (!d->ref.deref()) {
        delete d;
    }
}

GeoDataFeaturePrivate* GeoDataFeature::p() const
{
    return static_cast<GeoDataFeaturePrivate*>(d);
}

GeoDataFeature& GeoDataFeature::operator=( const GeoDataFeature& other )
{
    GeoDataObject::operator=( other );
    
    if (!d->ref.deref()) {
        delete d;
    }

    d = other.d;
    d->ref.ref();
    
    return *this;
}

QString GeoDataFeature::nodeType() const
{
    return p()->nodeType();
}

EnumFeatureId GeoDataFeature::featureId() const
{
    return d->featureId();
}

void GeoDataFeature::initializeDefaultStyles()
{
    // We need to do this similar to the way KCmdLineOptions works in
    // the future: Having a PlacemarkStyleProperty properties[] would
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
              QFont( defaultFamily, defaultSize, 50, false ), QColor( Qt::white ) );

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

    s_defaultStyle[Observatory]
        = new GeoDataStyle( QPixmap( MarbleDirs::path( "bitmaps/observatory.png" ) ),
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
    return d->m_name;
}

void GeoDataFeature::setName( const QString &value )
{
    detach();
    d->m_name = value;
}

QString GeoDataFeature::address() const
{
    return d->m_address;
}

void GeoDataFeature::setAddress( const QString &value)
{
    detach();
    d->m_address = value;
}

QString GeoDataFeature::phoneNumber() const
{
    return d->m_phoneNumber;
}

void GeoDataFeature::setPhoneNumber( const QString &value)
{
    detach();
    d->m_phoneNumber = value;
}

QString GeoDataFeature::description() const
{
    return d->m_description;
}

void GeoDataFeature::setDescription( const QString &value)
{
    detach();
    d->m_description = value;
}

bool GeoDataFeature::descriptionIsCDATA() const
{
    return d->m_descriptionCDATA;
}

void GeoDataFeature::setDescriptionCDATA( bool cdata )
{
    detach();
    d->m_descriptionCDATA = cdata;
}

GeoDataAbstractView GeoDataFeature::abstractView() const
{
    return d->m_abstractView;
}

void GeoDataFeature::setAbstractView( const GeoDataAbstractView &abstractView )
{
    detach();
    d->m_abstractView = abstractView;
}

QString GeoDataFeature::styleUrl() const
{
    return d->m_styleUrl;
}

void GeoDataFeature::setStyleUrl( const QString &value)
{
    detach();
    d->m_styleUrl = value;
}

bool GeoDataFeature::isVisible() const
{
    return d->m_visible;
}

void GeoDataFeature::setVisible( bool value )
{
    detach();
    d->m_visible = value;
}

GeoDataTimeSpan& GeoDataFeature::timeSpan() const
{
    return d->m_timeSpan;
}

void GeoDataFeature::setTimeSpan( GeoDataTimeSpan timeSpan )
{
    detach();
    d->m_timeSpan = timeSpan;
}

GeoDataTimeStamp&  GeoDataFeature::timeStamp() const
{
    return d->m_timeStamp;
}

void GeoDataFeature::setTimeStamp( GeoDataTimeStamp timeStamp )
{
    detach();
    d->m_timeStamp = timeStamp;
}

GeoDataStyle* GeoDataFeature::style() const
{
    if ( d->m_style != 0 ) {
        return d->m_style;
    } else
    {
        if ( s_defaultStyleInitialized == false )
            initializeDefaultStyles();

        if ( d->m_visualCategory != None )
        {
            return s_defaultStyle[ d->m_visualCategory ];
        }
        else
        {
            // This should not happen
            mDebug() << "No Style got assigned!";
            return new GeoDataStyle( QPixmap(), 
                                     QFont( "Sans Serif",  8, 50, false ), QColor( Qt::black ) );
        }
    }
}

void GeoDataFeature::setStyle( GeoDataStyle* style )
{
    detach();
    d->m_style = style;
}

GeoDataExtendedData& GeoDataFeature::extendedData() const
{
    return d->m_extendedData;
}

void GeoDataFeature::setExtendedData( const GeoDataExtendedData& extendedData )
{
    detach();
    d->m_extendedData = extendedData;
}

GeoDataRegion& GeoDataFeature::region() const
{
    return d->m_region;
}

void GeoDataFeature::setRegion( const GeoDataRegion& region )
{
    detach();
    d->m_region = region;
}

GeoDataFeature::GeoDataVisualCategory GeoDataFeature::visualCategory() const
{
    return d->m_visualCategory;
}

void GeoDataFeature::setVisualCategory( GeoDataFeature::GeoDataVisualCategory index )
{
    detach();
    d->m_visualCategory = index;
}

const QString GeoDataFeature::role() const
{
    return d->m_role;
}

void GeoDataFeature::setRole( const QString &role )
{
    detach();
    d->m_role = role;
}

GeoDataStyleMap* GeoDataFeature::styleMap() const
{
    return d->m_styleMap;
}

void GeoDataFeature::setStyleMap( GeoDataStyleMap* styleMap )
{
    d->m_styleMap = styleMap;
}

int GeoDataFeature::popularityIndex() const
{
    return d->m_popularityIndex;
}

void GeoDataFeature::setPopularityIndex( int popularityIndex )
{
    detach();
    d->m_popularityIndex = popularityIndex;
}

qint64 GeoDataFeature::popularity() const
{
    return d->m_popularity;
}

void GeoDataFeature::setPopularity( qint64 popularity )
{
    detach();
    d->m_popularity = popularity;
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
    if(d->ref == 1)
        return;

    GeoDataFeaturePrivate* new_d = static_cast<GeoDataFeaturePrivate*>(d->copy());

    if (!d->ref.deref()) {
        delete d;
    }

    d = new_d;
    
    d->ref.ref();
}

void GeoDataFeature::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_name;
    stream << d->m_address;
    stream << d->m_phoneNumber;
    stream << d->m_description;
    stream << d->m_visible;
//    stream << d->m_visualCategory;
    stream << d->m_role;
    stream << d->m_popularity;
    stream << d->m_popularityIndex;
}

void GeoDataFeature::unpack( QDataStream& stream )
{
    detach();
    GeoDataObject::unpack( stream );

    stream >> d->m_name;
    stream >> d->m_address;
    stream >> d->m_phoneNumber;
    stream >> d->m_description;
    stream >> d->m_visible;
//    stream >> (int)d->m_visualCategory;
    stream >> d->m_role;
    stream >> d->m_popularity;
    stream >> d->m_popularityIndex;
}

}
