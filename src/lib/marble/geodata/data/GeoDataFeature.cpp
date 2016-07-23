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

#include <QDataStream>
#include <QSize>

#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"

#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataRegion.h"
#include "GeoDataCamera.h"

namespace Marble
{

const QSharedPointer<const GeoDataStyle> GeoDataFeaturePrivate::s_defaultStyle(new GeoDataStyle);

GeoDataFeature::GeoDataFeature()
    : d( new GeoDataFeaturePrivate() )
{
    d->ref.ref();
}

GeoDataFeature::GeoDataFeature( const GeoDataFeature& other )
    : GeoDataObject(),
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
    if (!d->ref.deref()) {
        delete d;
    }

    d = other.d;
    d->ref.ref();

    return *this;
}

bool GeoDataFeature::equals( const GeoDataFeature &other ) const
{
    if ( !GeoDataObject::equals(other) ||
         p()->m_name != other.p()->m_name ||
         p()->m_snippet != other.p()->m_snippet ||
         p()->m_description != other.p()->m_description ||
         p()->m_descriptionCDATA != other.p()->m_descriptionCDATA ||
         p()->m_address != other.p()->m_address ||
         p()->m_phoneNumber != other.p()->m_phoneNumber ||
         p()->m_styleUrl != other.p()->m_styleUrl ||
         p()->m_popularity != other.p()->m_popularity ||
         p()->m_zoomLevel != other.p()->m_zoomLevel ||
         p()->m_visible != other.p()->m_visible ||
         p()->m_role != other.p()->m_role ||
         p()->m_extendedData != other.p()->m_extendedData ||
         p()->m_timeSpan != other.p()->m_timeSpan ||
         p()->m_timeStamp != other.p()->m_timeStamp ||
         p()->m_region != other.p()->m_region ||
         *style() != *other.style() ) {
        return false;
    }

    if ( (!p()->m_styleMap && other.p()->m_styleMap) ||
         (p()->m_styleMap && !other.p()->m_styleMap) ) {
        return false;
    }

    if ( (p()->m_styleMap && other.p()->m_styleMap) &&
         (*p()->m_styleMap != *other.p()->m_styleMap) ) {
        return false;
    }

    if ( !p()->m_abstractView && !other.p()->m_abstractView ) {
        return true;
    } else if ( (!p()->m_abstractView && other.p()->m_abstractView) ||
                (p()->m_abstractView && !other.p()->m_abstractView) ) {
        return false;
    }

    if ( p()->m_abstractView->nodeType() != other.p()->m_abstractView->nodeType() ) {
        return false;
    }

    if ( p()->m_abstractView->nodeType() == GeoDataTypes::GeoDataCameraType ) {
        GeoDataCamera *thisCam = dynamic_cast<GeoDataCamera*>( p()->m_abstractView );
        GeoDataCamera *otherCam = dynamic_cast<GeoDataCamera*>( other.p()->m_abstractView );
        Q_ASSERT(thisCam && otherCam);

        if ( *thisCam != *otherCam ) {
            return false;
        }
    } else if ( p()->m_abstractView->nodeType() == GeoDataTypes::GeoDataLookAtType ) {
        GeoDataLookAt *thisLookAt = dynamic_cast<GeoDataLookAt*>( p()->m_abstractView );
        GeoDataLookAt *otherLookAt = dynamic_cast<GeoDataLookAt*>( other.p()->m_abstractView );
        Q_ASSERT(thisLookAt && otherLookAt);

        if ( *thisLookAt != *otherLookAt ) {
            return false;
        }
    }

    return true;
}

const char* GeoDataFeature::nodeType() const
{
    return p()->nodeType();
}

EnumFeatureId GeoDataFeature::featureId() const
{
    return d->featureId();
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

GeoDataSnippet GeoDataFeature::snippet() const
{
    return d->m_snippet;
}

void GeoDataFeature::setSnippet( const GeoDataSnippet &snippet )
{
    detach();
    d->m_snippet = snippet;
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

const GeoDataAbstractView* GeoDataFeature::abstractView() const
{
    return d->m_abstractView;
}

GeoDataAbstractView *GeoDataFeature::abstractView()
{
    // FIXME: Calling detach() doesn't help at all because the m_abstractView
    // object isn't actually copied in the Private class as well.
    // detach();
    return d->m_abstractView;
}

void GeoDataFeature::setAbstractView( GeoDataAbstractView *abstractView )
{
    detach();
    d->m_abstractView = abstractView;
}

QString GeoDataFeature::styleUrl() const
{
    return d->m_styleUrl;
}

void GeoDataFeature::setStyleUrl( const QString &value )
{
    detach();

    d->m_styleUrl = value;

    if ( value.isEmpty() ) {
        d->m_style = GeoDataStyle::Ptr();
        return;
    }

    QString styleUrl = value;
    styleUrl.remove('#');
    GeoDataObject *object = parent();
    bool found = false;
    while ( object && !found ) {
        if( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataDocument *doc = static_cast<GeoDataDocument*> ( object );
            GeoDataStyleMap &styleMap = doc->styleMap( styleUrl );
            if( !styleMap.value( QString( "normal" ) ).isEmpty() ) {
                styleUrl = styleMap.value( QString( "normal" ) );
                styleUrl.remove('#');
            }
            // Not calling setStyle here because we don't want
            // re-parenting of the style
            d->m_style = doc->style( styleUrl );
            found = true;
        }
        object = object->parent();
    }
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

bool GeoDataFeature::isGloballyVisible() const
{
    if ( parent() == 0 ) {
        return d->m_visible;
    }
    GeoDataContainer *container = static_cast<GeoDataContainer*>( parent() );
    return d->m_visible && container->isGloballyVisible();
}


const GeoDataTimeSpan &GeoDataFeature::timeSpan() const
{
    return d->m_timeSpan;
}

GeoDataTimeSpan &GeoDataFeature::timeSpan()
{
    detach();
    return d->m_timeSpan;
}

void GeoDataFeature::setTimeSpan( const GeoDataTimeSpan &timeSpan )
{
    detach();
    d->m_timeSpan = timeSpan;
}

const GeoDataTimeStamp &GeoDataFeature::timeStamp() const
{
    return d->m_timeStamp;
}

GeoDataTimeStamp &GeoDataFeature::timeStamp()
{
    detach();
    return d->m_timeStamp;
}

void GeoDataFeature::setTimeStamp( const GeoDataTimeStamp &timeStamp )
{
    detach();
    d->m_timeStamp = timeStamp;
}

const GeoDataExtendedData &GeoDataFeature::extendedData() const
{
    return d->m_extendedData;
}

GeoDataStyle::ConstPtr GeoDataFeature::style() const
{
    if (d->m_style) {
        return d->m_style;
    }

    return GeoDataFeaturePrivate::s_defaultStyle;
}

GeoDataStyle::ConstPtr GeoDataFeature::customStyle() const
{
    return d->m_style;
}

void GeoDataFeature::setStyle( const GeoDataStyle::Ptr &style )
{
    detach();
    if (style)
        style->setParent( this );
    d->m_style = style;
}

GeoDataExtendedData& GeoDataFeature::extendedData()
{
    detach();
    return d->m_extendedData;
}

void GeoDataFeature::setExtendedData( const GeoDataExtendedData& extendedData )
{
    detach();
    d->m_extendedData = extendedData;
}

GeoDataRegion& GeoDataFeature::region() const
{
    // FIXME: Should call detach(). Maybe don't return reference.
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

const GeoDataStyleMap* GeoDataFeature::styleMap() const
{
    return d->m_styleMap;
}

void GeoDataFeature::setStyleMap( const GeoDataStyleMap* styleMap )
{
    d->m_styleMap = styleMap;
}

int GeoDataFeature::zoomLevel() const
{
    return d->m_zoomLevel;
}

void GeoDataFeature::setZoomLevel( int zoomLevel )
{
    detach();
    d->m_zoomLevel = zoomLevel;
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

void GeoDataFeature::detach()
{
    if(d->ref.load() == 1) {
        return;
    }

    GeoDataFeaturePrivate* new_d = d->copy();

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
    stream << d->m_zoomLevel;
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
    stream >> d->m_zoomLevel;
}

}
