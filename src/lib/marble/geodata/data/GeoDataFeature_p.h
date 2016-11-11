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

#include "GeoDataExtendedData.h"
#include "GeoDataAbstractView.h"
#include "GeoDataFeature.h"
#include "GeoDataRegion.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataTypes.h"
#include "GeoDataStyle.h"
#include "GeoDataSnippet.h"
#include "GeoDataLookAt.h"
#include "GeoDataCamera.h"

namespace Marble
{

class GeoDataFeatureExtendedData
{
public:
    GeoDataSnippet      m_snippet;      // Snippet of the feature.
    QString             m_description;  // A longer textual description
    bool                m_descriptionCDATA; // True if description should be considered CDATA
    QString             m_address;      // The address.  Optional
    QString             m_phoneNumber;  // Phone         Optional
    GeoDataAbstractView* m_abstractView; // AbstractView  Optional
    GeoDataTimeSpan  m_timeSpan;
    GeoDataTimeStamp m_timeStamp;
    GeoDataRegion m_region;

    GeoDataFeatureExtendedData() :
        m_snippet(),
        m_description(),
        m_descriptionCDATA(false),
        m_address(),
        m_phoneNumber(),
        m_abstractView( 0 ),
        m_timeSpan(),
        m_timeStamp(),
        m_region()
    {
        // nothing to do
    }

    GeoDataFeatureExtendedData(const GeoDataFeatureExtendedData &other) :
        m_snippet( other.m_snippet ),
        m_description( other.m_description ),
        m_descriptionCDATA( other.m_descriptionCDATA),
        m_address( other.m_address ),
        m_phoneNumber( other.m_phoneNumber ),
        m_abstractView( other.m_abstractView ),
        m_timeSpan( other.m_timeSpan ),
        m_timeStamp( other.m_timeStamp ),
        m_region( other.m_region )
    {
        // nothing to do
    }

    GeoDataFeatureExtendedData& operator=(const GeoDataFeatureExtendedData &other)
    {
        m_snippet = other.m_snippet;
        m_description = other.m_description;
        m_descriptionCDATA = other.m_descriptionCDATA;
        m_address = other.m_address;
        m_phoneNumber = other.m_phoneNumber;
        m_abstractView = other.m_abstractView;
        m_timeSpan = other.m_timeSpan;
        m_timeStamp = other.m_timeStamp;
        m_region = other.m_region;
        return *this;
    }

    bool operator==(const GeoDataFeatureExtendedData &other) const
    {
        if (m_snippet != other.m_snippet ||
                m_description != other.m_description ||
                m_descriptionCDATA != other.m_descriptionCDATA ||
                m_address != other.m_address ||
                m_phoneNumber != other.m_phoneNumber ||
                m_timeSpan != other.m_timeSpan ||
                m_timeStamp != other.m_timeStamp ||
                m_region != other.m_region) {
            return false;
        }

        if ( (!m_abstractView && other.m_abstractView) ||
                    (m_abstractView && !other.m_abstractView) ) {
            return false;
        }

        if (m_abstractView && other.m_abstractView) {
            if ( m_abstractView->nodeType() != other.m_abstractView->nodeType() ) {
                return false;
            }

            if ( m_abstractView->nodeType() == GeoDataTypes::GeoDataCameraType ) {
                GeoDataCamera *thisCam = dynamic_cast<GeoDataCamera*>( m_abstractView );
                GeoDataCamera *otherCam = dynamic_cast<GeoDataCamera*>( other.m_abstractView );
                Q_ASSERT(thisCam && otherCam);

                if ( *thisCam != *otherCam ) {
                    return false;
                }
            } else if ( m_abstractView->nodeType() == GeoDataTypes::GeoDataLookAtType ) {
                GeoDataLookAt *thisLookAt = dynamic_cast<GeoDataLookAt*>( m_abstractView );
                GeoDataLookAt *otherLookAt = dynamic_cast<GeoDataLookAt*>( other.m_abstractView );
                Q_ASSERT(thisLookAt && otherLookAt);

                if ( *thisLookAt != *otherLookAt ) {
                    return false;
                }
            }
        }

        return true;
    }

    bool operator!=(const GeoDataFeatureExtendedData &other) const
    {
        return !(*this == other);
    }
};

class GeoDataFeaturePrivate
{
  public:
    GeoDataFeaturePrivate() :
        m_name(),
        m_styleUrl(),
        m_popularity( 0 ),
        m_zoomLevel( 1 ),
        m_visible( true ),
        m_role(" "),
        m_style( 0 ),
        m_styleMap( 0 ),
        m_extendedData(),
        m_featureExtendedData(nullptr)
    {
    }

    GeoDataFeaturePrivate( const GeoDataFeaturePrivate& other ) :
        m_name( other.m_name ),
        m_styleUrl( other.m_styleUrl ),
        m_popularity( other.m_popularity ),
        m_zoomLevel( other.m_zoomLevel ),
        m_visible( other.m_visible ),
        m_role( other.m_role ),
        m_style( other.m_style ),               //FIXME: both style and stylemap need to be reworked internally!!!!
        m_styleMap( other.m_styleMap ),
        m_extendedData( other.m_extendedData ),
        m_featureExtendedData(nullptr)
    {
        if (other.m_featureExtendedData) {
            m_featureExtendedData = new GeoDataFeatureExtendedData(*other.m_featureExtendedData);
        }
    }

    GeoDataFeaturePrivate& operator=( const GeoDataFeaturePrivate& other )
    {
        m_name = other.m_name;
        m_styleUrl = other.m_styleUrl;
        m_popularity = other.m_popularity;
        m_zoomLevel = other.m_zoomLevel;
        m_visible = other.m_visible;
        m_role = other.m_role;
        m_style = other.m_style;
        m_styleMap = other.m_styleMap;
        m_extendedData = other.m_extendedData;
        delete m_featureExtendedData;
        m_featureExtendedData = nullptr;
        if (other.m_featureExtendedData) {
            m_featureExtendedData = new GeoDataFeatureExtendedData(*other.m_featureExtendedData);
        }
        return *this;
    }

    virtual EnumFeatureId featureId() const
    {
        return InvalidFeatureId;
    }

    virtual ~GeoDataFeaturePrivate()
    {
        delete m_featureExtendedData;
    }

    GeoDataFeatureExtendedData & featureExtendedData()
    {
        if (!m_featureExtendedData) {
            m_featureExtendedData = new GeoDataFeatureExtendedData;
        }
        return *m_featureExtendedData;
    }
    const GeoDataFeatureExtendedData & featureExtendedData() const
    {
        if (!m_featureExtendedData) {
            m_featureExtendedData = new GeoDataFeatureExtendedData;
        }
        return *m_featureExtendedData;
    }

    QString             m_name;         // Name of the feature. Is shown on screen
    QString             m_styleUrl;     // styleUrl     Url#tag to a document wide style
    qint64              m_popularity;   // Population/Area/Altitude depending on placemark(!)
    int                 m_zoomLevel;    // Zoom Level of the feature

    bool        m_visible;      // True if this feature should be shown.

    QString       m_role;

    GeoDataStyle::Ptr m_style;
    const GeoDataStyleMap* m_styleMap;

    GeoDataExtendedData m_extendedData;
    mutable GeoDataFeatureExtendedData* m_featureExtendedData;

    // Static members
    static const QSharedPointer<const GeoDataStyle> s_defaultStyle;
};


} // namespace Marble

#endif

