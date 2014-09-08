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
#include <QImage>

// Marble
#include "MarbleDebug.h"
#include "GeoDataFeature.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataNetworkLink.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataTour.h"


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

GeoDataContainerPrivate* GeoDataContainer::p()
{
    return static_cast<GeoDataContainerPrivate*>(d);
}

const GeoDataContainerPrivate* GeoDataContainer::p() const
{
    return static_cast<GeoDataContainerPrivate*>(d);
}

bool GeoDataContainer::equals( const GeoDataContainer &other ) const
{
    if ( !GeoDataFeature::equals(other) ) {
        return false;
    }

    QVector<GeoDataFeature*>::const_iterator thisBegin = p()->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator thisEnd = p()->m_vector.constEnd();
    QVector<GeoDataFeature*>::const_iterator otherBegin = other.p()->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator otherEnd = other.p()->m_vector.constEnd();

    for (; thisBegin != thisEnd && otherBegin != otherEnd; ++thisBegin, ++otherBegin) {
        if ( (*thisBegin)->nodeType() != (*otherBegin)->nodeType() ) {
            return false;
        }

        if ( (*thisBegin)->nodeType() ==  GeoDataTypes::GeoDataDocumentType ) {
            GeoDataDocument *thisDoc = static_cast<GeoDataDocument*>( *thisBegin );
            GeoDataDocument *otherDoc = static_cast<GeoDataDocument*>( *otherBegin );
            Q_ASSERT( thisDoc && otherDoc );

            if ( *thisDoc != *otherDoc ) {
                return false;
            }
        } else if ( (*thisBegin)->nodeType() == GeoDataTypes::GeoDataFolderType ) {
            GeoDataFolder *thisFolder = static_cast<GeoDataFolder*>( *thisBegin );
            GeoDataFolder *otherFolder = static_cast<GeoDataFolder*>( *otherBegin );
            Q_ASSERT( thisFolder && otherFolder );

            if ( *thisFolder != *otherFolder ) {
                return false;
            }
        } else if ( (*thisBegin)->nodeType() == GeoDataTypes::GeoDataNetworkLinkControlType ) {
            GeoDataNetworkLinkControl *thisNLC = static_cast<GeoDataNetworkLinkControl*>( *thisBegin );
            GeoDataNetworkLinkControl *otherNLC = static_cast<GeoDataNetworkLinkControl*>( *otherBegin );
            Q_ASSERT( thisNLC && otherNLC );

            if ( *thisNLC != *otherNLC ) {
                return false;
            }
        } else if ( (*thisBegin)->nodeType() == GeoDataTypes::GeoDataNetworkLinkType ) {
            GeoDataNetworkLink *thisNetLink = static_cast<GeoDataNetworkLink*>( *thisBegin );
            GeoDataNetworkLink *otherNetLink = static_cast<GeoDataNetworkLink*>( *otherBegin );
            Q_ASSERT( thisNetLink && otherNetLink );

            if ( *thisNetLink != *otherNetLink ) {
                return false;
            }
        } else if ( (*thisBegin)->nodeType() == GeoDataTypes::GeoDataGroundOverlayType ) {
            GeoDataGroundOverlay *thisGO = static_cast<GeoDataGroundOverlay*>( *thisBegin );
            GeoDataGroundOverlay *otherGO = static_cast<GeoDataGroundOverlay*>( *otherBegin );
            Q_ASSERT( thisGO && otherGO );

            if ( *thisGO != *otherGO ) {
                return false;
            }
        } else if ( (*thisBegin)->nodeType() == GeoDataTypes::GeoDataPhotoOverlayType ) {
            GeoDataPhotoOverlay *thisPO = static_cast<GeoDataPhotoOverlay*>( *thisBegin );
            GeoDataPhotoOverlay *otherPO = static_cast<GeoDataPhotoOverlay*>( *otherBegin );
            Q_ASSERT( thisPO && otherPO );

            if ( *thisPO != *otherPO ) {
                return false;
            }
        } else if ( (*thisBegin)->nodeType() == GeoDataTypes::GeoDataScreenOverlayType ) {
            GeoDataScreenOverlay *thisSO = static_cast<GeoDataScreenOverlay*>( *thisBegin );
            GeoDataScreenOverlay *otherSO = static_cast<GeoDataScreenOverlay*>( *otherBegin );
            Q_ASSERT( thisSO && otherSO );

            if ( *thisSO != *otherSO ) {
                return false;
            }
        } else if ( (*thisBegin)->nodeType() == GeoDataTypes::GeoDataTourType ) {
            GeoDataTour *thisTour = static_cast<GeoDataTour*>( *thisBegin );
            GeoDataTour *otherTour = static_cast<GeoDataTour*>( *otherBegin );
            Q_ASSERT( thisTour && otherTour );

            if ( *thisTour != *otherTour ) {
                return false;
            }
        } else if ( (*thisBegin)->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *thisPM = static_cast<GeoDataPlacemark*>( *thisBegin );
            GeoDataPlacemark *otherPM = static_cast<GeoDataPlacemark*>( *otherBegin );
            Q_ASSERT( thisPM && otherPM );

            if ( *thisPM != *otherPM ) {
                return false;
            }
        }
    }

    return thisBegin == thisEnd && otherBegin == otherEnd;
}

GeoDataLatLonAltBox GeoDataContainer::latLonAltBox() const
{
    GeoDataLatLonAltBox result;

    QVector<GeoDataFeature*>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator end = p()->m_vector.constEnd();
    for (; it != end; ++it) {

        // Get all the placemarks from GeoDataContainer
        if ( (*it)->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(*it);

            // Only use visible placemarks for extracting their latLonAltBox and
            // making an union with the global latLonAltBox Marble will fit its
            // zoom to
            if (placemark->isVisible())
            {
                if (result.isEmpty()) {
                    result = placemark->geometry()->latLonAltBox();
                } else {
                    result |= placemark->geometry()->latLonAltBox();
                }
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
    detach();
    return p()->m_vector.at(i);
}

const GeoDataFeature* GeoDataContainer::child( int i ) const
{
    return p()->m_vector.at(i);
}

/**
 * @brief returns the position of an item in the list
 */
int GeoDataContainer::childPosition( const GeoDataFeature* object ) const
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


void GeoDataContainer::insert( GeoDataFeature *other, int index )
{
    insert( index, other );
}

void GeoDataContainer::insert( int index, GeoDataFeature *feature )
{
    detach();
    feature->setParent(this);
    p()->m_vector.insert( index, feature );
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
    detach();
    return p()->m_vector.begin();
}

QVector<GeoDataFeature*>::Iterator GeoDataContainer::end()
{
    detach();
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
