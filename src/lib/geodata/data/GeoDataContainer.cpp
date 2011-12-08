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


// Own
#include "GeoDataContainer.h"
#include "GeoDataContainer_p.h"

// Qt
#include <QtGui/QImage>

// Marble
#include "MarbleDebug.h"
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

GeoDataContainer::~GeoDataContainer()
{
}

GeoDataContainerPrivate* GeoDataContainer::p() const
{
    return static_cast<GeoDataContainerPrivate*>(d);
}

GeoDataLatLonAltBox GeoDataContainer::latLonAltBox() const
{
    GeoDataLatLonAltBox result;

    QVector<GeoDataFeature*>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator end = p()->m_vector.constEnd();
    for (; it != end; ++it) {
        if ( (*it)->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(*it);
            if (result.isEmpty()) {
                result = placemark->geometry()->latLonAltBox();
            } else {
                result |= placemark->geometry()->latLonAltBox();
            }
        }
        else if ( (*it)->nodeType() == GeoDataTypes::GeoDataFolderType
                 || (*it)->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataContainer *container = static_cast<GeoDataContainer*>(*it);
            if (result.isEmpty()) {
                result = container->latLonAltBox();
            } else {
                result |= container->latLonAltBox();
            }
        }
    }
    return result;
}

QVector<GeoDataFolder*> GeoDataContainer::folderList() const
{
    QVector<GeoDataFolder*> results;

    QVector<GeoDataFeature*>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator end = p()->m_vector.constEnd();

    for (; it != end; ++it) {
        GeoDataFolder *folder = dynamic_cast<GeoDataFolder*>(*it);
        if ( folder ) {
            results.append( folder );
        }
    }

    return results;
}

QVector<GeoDataPlacemark*> GeoDataContainer::placemarkList() const
{
    QVector<GeoDataPlacemark*> results;

    QVector<GeoDataFeature*>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator end = p()->m_vector.constEnd();

    for (; it != end; ++it) {
        GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( *it );
        if ( placemark ) {
            results.append( placemark );
        }
    }

    return results;
}

QVector<GeoDataFeature*> GeoDataContainer::featureList() const
{
    return p()->m_vector;
}

/**
 * @brief  returns the requested child item
 */
GeoDataFeature* GeoDataContainer::child( int i )
{
    return p()->m_vector.at(i);
}

const GeoDataFeature* GeoDataContainer::child( int i ) const
{
    return p()->m_vector.at(i);
}

/**
 * @brief returns the position of an item in the list
 */
int GeoDataContainer::childPosition( GeoDataFeature* object )
{
    for ( int i=0; i< p()->m_vector.size(); i++ )
    {
        if ( p()->m_vector.at( i ) == object )
        {
            return i;
        }
    }
    return -1;
}


void GeoDataContainer::append( GeoDataFeature *other )
{
    detach();
    other->setParent(this);
    p()->m_vector.append( other );
}


void GeoDataContainer::remove( int index )
{
    detach();
    p()->m_vector.remove( index );
}

int GeoDataContainer::size() const
{
    return p()->m_vector.size();
}

GeoDataFeature& GeoDataContainer::at( int pos )
{
    detach();
    return *(p()->m_vector[ pos ]);
}

const GeoDataFeature& GeoDataContainer::at( int pos ) const
{
    return *(p()->m_vector.at( pos ));
}

GeoDataFeature& GeoDataContainer::last()
{
    detach();
    return *(p()->m_vector.last());
}

const GeoDataFeature& GeoDataContainer::last() const
{
    return *(p()->m_vector.last());
}

GeoDataFeature& GeoDataContainer::first()
{
    detach();
    return *(p()->m_vector.first());
}

const GeoDataFeature& GeoDataContainer::first() const
{
    return *(p()->m_vector.first());
}

void GeoDataContainer::clear()
{
    GeoDataContainer::detach();
    qDeleteAll(p()->m_vector);
    p()->m_vector.clear();
}

QVector<GeoDataFeature*>::Iterator GeoDataContainer::begin()
{
    return p()->m_vector.begin();
}

QVector<GeoDataFeature*>::Iterator GeoDataContainer::end()
{
    return p()->m_vector.end();
}

QVector<GeoDataFeature*>::ConstIterator GeoDataContainer::constBegin() const
{
    return p()->m_vector.constBegin();
}

QVector<GeoDataFeature*>::ConstIterator GeoDataContainer::constEnd() const
{
    return p()->m_vector.constEnd();
}

void GeoDataContainer::pack( QDataStream& stream ) const
{
    GeoDataFeature::pack( stream );

    stream << p()->m_vector.count();

    for ( QVector <GeoDataFeature*>::const_iterator iterator = p()->m_vector.constBegin();
          iterator != p()->m_vector.constEnd();
          ++iterator )
    {
        const GeoDataFeature *feature = *iterator;
        stream << feature->featureId();
        feature->pack( stream );
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
                GeoDataFolder *folder = new GeoDataFolder;
                folder->unpack( stream );
                p()->m_vector.append( folder );
                }
                break;
            case GeoDataPlacemarkId:
                {
                GeoDataPlacemark *placemark = new GeoDataPlacemark;
                placemark->unpack( stream );
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
