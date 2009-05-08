//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef GEODATAFEATUREPRIVATE_H
#define GEODATAFEATUREPRIVATE_H

#include <QtCore/QString>
#if QT_VERSION < 0x040400
# include <qatomic.h>
#else
# include <QtCore/QAtomicInt>
#endif

#include "GeoDataFeature.h"

namespace Marble
{
#if QT_VERSION < 0x040400
    typedef QAtomic QAtomicInt;
#endif

class GeoDataFeaturePrivate
{
  public:
    GeoDataFeaturePrivate() :
        m_popularity( 0 ),
        m_popularityIndex( 19 ),
        m_visible( true ),
        m_visualCategory( GeoDataFeature::Unknown ),
        m_role(' '),
        m_style( 0 ),
        m_styleMap( 0 ),
        ref( 0 )
    {
    }

    GeoDataFeaturePrivate( const GeoDataFeaturePrivate& other ) :
        m_popularity( other.m_popularity ),
        m_popularityIndex( other.m_popularityIndex ),
        m_visible( other.m_visible ),
        m_visualCategory( other.m_visualCategory ),
        m_role( other.m_role ),
        m_style( other.m_style ),               //FIXME: both style and stylemap need to be reworked internally!!!!
        m_styleMap( other.m_styleMap ),
        ref( 0 )
    {
    }

    void operator=( const GeoDataFeaturePrivate& other )
    {
        m_name = other.m_name;
        m_description = other.m_description;
        m_address = other.m_address;
        m_phoneNumber = other.m_phoneNumber;
        m_styleUrl = other.m_styleUrl;
        m_popularity = other.m_popularity;
        m_popularityIndex = other.m_popularityIndex;
        m_visible = other.m_visible;
        m_role = other.m_role;
        m_style = other.m_style;
        m_styleMap = other.m_styleMap;
        m_visualCategory = other.m_visualCategory;
    }
    
    virtual void* copy() 
    { 
        GeoDataFeaturePrivate* copy = new GeoDataFeaturePrivate;
        *copy = *this;
        return copy;
    };

    virtual EnumFeatureId featureId() const
    {
        return InvalidFeatureId;
    }

    virtual ~GeoDataFeaturePrivate()
    {
    }

    QString     m_name;         // Name of the feature. Is shown on screen
    QString     m_description;  // A longer textual description
    QString     m_address;      // The address.  Optional
    QString     m_phoneNumber;  // Phone         Optional
    QString     m_styleUrl;     // styleUrl     Url#tag to a document wide style
    qint64      m_popularity;   // Population(!)
    int         m_popularityIndex; // Index of population

    bool        m_visible;      // True if this feature should be shown.
    GeoDataFeature::GeoDataVisualCategory  m_visualCategory; // the visual category


    QChar       m_role;

    GeoDataStyle* m_style;
    GeoDataStyleMap* m_styleMap;
    
    QAtomicInt  ref;
};

} // namespace Marble

#endif //GEODATAFEATUREPRIVATE_H
