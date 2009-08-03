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


// Own
#include "GeoDataContainer.h"
#include "GeoDataContainer_p.h"

// Qt
#include <QtCore/QDebug>
#include <QtGui/QImage>

// Marble
#include "GeoDataFeature.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"

namespace Marble
{

GeoDataContainer::GeoDataContainer()
    : GeoDataFeature( new GeoDataContainerPrivate )
{
}

GeoDataContainer::GeoDataContainer( GeoDataContainerPrivate *priv )
    : GeoDataFeature( priv )
{
}

GeoDataContainer::GeoDataContainer( const GeoDataContainer& other )
    : GeoDataFeature( other )
{
}

GeoDataContainer::GeoDataContainer( const GeoDataFeature& other )
    : GeoDataFeature( other )
{
}

GeoDataContainer::~GeoDataContainer()
{
}

GeoDataContainerPrivate* GeoDataContainer::p() const
{
    return static_cast<GeoDataContainerPrivate*>(d);
}

QString GeoDataContainer::nodeType() const
{
    return p()->nodeType();
}

QVector<GeoDataFolder> GeoDataContainer::folders() const
{
    QVector<GeoDataFolder> results;

    QVector<GeoDataFeature>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataFeature>::const_iterator end = p()->m_vector.constEnd();

    for (; it != end; ++it) {
        GeoDataFeature feature = *it;

        if ( feature.featureId() == GeoDataFolderId )
            results.append( feature );
    }

    return results;
}

QVector<GeoDataPlacemark> GeoDataContainer::placemarks() const
{
    QVector<GeoDataPlacemark> results;

    QVector<GeoDataFeature>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataFeature>::const_iterator end = p()->m_vector.constEnd();

    for (; it != end; ++it) {
        if ( GeoDataPlacemarkId == it->featureId() ) {
            GeoDataPlacemark placemark = *it;
            results.append( placemark );
        }
    }

    return results;
}

QVector<GeoDataFeature>& GeoDataContainer::features() const
{
    return p()->m_vector;
}

void GeoDataContainer::append( const GeoDataFeature& other )
{
    detach();
    p()->m_vector.append( other );
}

int GeoDataContainer::size() const
{
    return p()->m_vector.size();
}

GeoDataFeature& GeoDataContainer::at( int pos )
{
    detach();
    return p()->m_vector[ pos ];
}

const GeoDataFeature& GeoDataContainer::at( int pos ) const
{
    return p()->m_vector.at( pos );
}

GeoDataFeature& GeoDataContainer::last()
{
    detach();
    return p()->m_vector.last();
}

const GeoDataFeature& GeoDataContainer::last() const
{
    return p()->m_vector.last();
}

GeoDataFeature& GeoDataContainer::first()
{
    detach();
    return p()->m_vector.first();
}

const GeoDataFeature& GeoDataContainer::first() const
{
    return p()->m_vector.first();
}

void GeoDataContainer::clear()
{
    GeoDataContainer::detach();
    p()->m_vector.clear();
}

QVector<GeoDataFeature>::Iterator GeoDataContainer::begin()
{
    GeoDataContainer::detach();
    return p()->m_vector.begin();
}

QVector<GeoDataFeature>::Iterator GeoDataContainer::end()
{
    GeoDataContainer::detach();
    return p()->m_vector.end();
}

QVector<GeoDataFeature>::ConstIterator GeoDataContainer::constBegin() const
{
    return p()->m_vector.constBegin();
}

QVector<GeoDataFeature>::ConstIterator GeoDataContainer::constEnd() const
{
    return p()->m_vector.constEnd();
}

void GeoDataContainer::pack( QDataStream& stream ) const
{
    GeoDataFeature::pack( stream );

    stream << p()->m_vector.count();

    for ( QVector <GeoDataFeature>::const_iterator iterator = p()->m_vector.constBegin();
          iterator != p()->m_vector.constEnd();
          ++iterator )
    {
        const GeoDataFeature& feature = *iterator;
        stream << feature.featureId();
        feature.pack( stream );
    }
}

void GeoDataContainer::unpack( QDataStream& stream )
{
    detach();
    GeoDataFeature::unpack( stream );

    int count;
    stream >> count;

    for ( int i = 0; i < count; ++i ) {
        int featureId;
        stream >> featureId;
        switch( featureId ) {
            case GeoDataDocumentId:
                /* not usable!!!! */ break;
            case GeoDataFolderId:
                {
                GeoDataFolder folder;
                folder.unpack( stream );
                p()->m_vector.append( folder );
                }
                break;
            case GeoDataPlacemarkId:
                {
                GeoDataPlacemark placemark;
                placemark.unpack( stream );
                p()->m_vector.append( placemark );
                }
                break;
            case GeoDataNetworkLinkId:
                break;
            case GeoDataScreenOverlayId:
                break;
            case GeoDataGroundOverlayId:
                break;
            default: break;
        };
    }
}

}
