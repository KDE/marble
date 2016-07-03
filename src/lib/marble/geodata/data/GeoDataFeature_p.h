//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAFEATUREPRIVATE_H
#define MARBLE_GEODATAFEATUREPRIVATE_H

#include <QString>
#include <QAtomicInt>

#include "GeoDataExtendedData.h"
#include "GeoDataAbstractView.h"
#include "GeoDataFeature.h"
#include "GeoDataRegion.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataTypes.h"
#include "GeoDataStyle.h"
#include "GeoDataSnippet.h"
#include "MarbleDirs.h"
#include "StyleBuilder.h"

namespace Marble
{

class GeoDataFeaturePrivate
{
  public:
    GeoDataFeaturePrivate() :
        m_name(),
        m_snippet(),
        m_description(),
        m_descriptionCDATA(),
        m_address(),
        m_phoneNumber(),
        m_styleUrl(),
        m_abstractView( 0 ),
        m_popularity( 0 ),
        m_zoomLevel( 1 ),
        m_visible( true ),
        m_visualCategory( GeoDataFeature::Default ),
        m_role(" "),
        m_style( 0 ),
        m_styleMap( 0 ),
        m_extendedData(),
        m_timeSpan(),
        m_timeStamp(),
        m_region(),
        ref( 0 )
    {
    }

    GeoDataFeaturePrivate( const GeoDataFeaturePrivate& other ) :
        m_name( other.m_name ),
        m_snippet( other.m_snippet ),
        m_description( other.m_description ),
        m_descriptionCDATA( other.m_descriptionCDATA),
        m_address( other.m_address ),
        m_phoneNumber( other.m_phoneNumber ),
        m_styleUrl( other.m_styleUrl ),
        m_abstractView( other.m_abstractView ),
        m_popularity( other.m_popularity ),
        m_zoomLevel( other.m_zoomLevel ),
        m_visible( other.m_visible ),
        m_visualCategory( other.m_visualCategory ),
        m_role( other.m_role ),
        m_style( other.m_style ),               //FIXME: both style and stylemap need to be reworked internally!!!!
        m_styleMap( other.m_styleMap ),
        m_extendedData( other.m_extendedData ),
        m_timeSpan( other.m_timeSpan ),
        m_timeStamp( other.m_timeStamp ),
        m_region( other.m_region ),
        ref( 0 )
    {
    }

    GeoDataFeaturePrivate& operator=( const GeoDataFeaturePrivate& other )
    {
        m_name = other.m_name;
        m_snippet = other.m_snippet;
        m_description = other.m_description;
        m_descriptionCDATA = other.m_descriptionCDATA;
        m_address = other.m_address;
        m_phoneNumber = other.m_phoneNumber;
        m_styleUrl = other.m_styleUrl;
        m_abstractView = other.m_abstractView;
        m_popularity = other.m_popularity;
        m_zoomLevel = other.m_zoomLevel;
        m_visible = other.m_visible;
        m_role = other.m_role;
        m_style = other.m_style;
        m_styleMap = other.m_styleMap;
        m_timeSpan = other.m_timeSpan;
        m_timeStamp = other.m_timeStamp;
        m_visualCategory = other.m_visualCategory;
        m_extendedData = other.m_extendedData;
        m_region = other.m_region;
        return *this;
    }
    
    virtual GeoDataFeaturePrivate* copy()
    { 
        GeoDataFeaturePrivate* copy = new GeoDataFeaturePrivate;
        *copy = *this;
        return copy;
    }

    virtual EnumFeatureId featureId() const
    {
        return InvalidFeatureId;
    }

    virtual ~GeoDataFeaturePrivate()
    {
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataFeatureType;
    }

    QString             m_name;         // Name of the feature. Is shown on screen
    GeoDataSnippet      m_snippet;      // Snippet of the feature.
    QString             m_description;  // A longer textual description
    bool                m_descriptionCDATA; // True if description should be considered CDATA
    QString             m_address;      // The address.  Optional
    QString             m_phoneNumber;  // Phone         Optional
    QString             m_styleUrl;     // styleUrl     Url#tag to a document wide style
    GeoDataAbstractView* m_abstractView; // AbstractView  Optional
    qint64              m_popularity;   // Population/Area/Altitude depending on placemark(!)
    int                 m_zoomLevel;    // Zoom Level of the feature

    bool        m_visible;      // True if this feature should be shown.
    GeoDataFeature::GeoDataVisualCategory  m_visualCategory; // the visual category


    QString       m_role;

    GeoDataStyle::Ptr m_style;
    const GeoDataStyleMap* m_styleMap;

    GeoDataExtendedData m_extendedData;

    GeoDataTimeSpan  m_timeSpan;
    GeoDataTimeStamp m_timeStamp;

    GeoDataRegion m_region;
    
    QAtomicInt  ref;

    // Static members
    static const QSharedPointer<const GeoDataStyle> s_defaultStyle;
};

} // namespace Marble

#endif

