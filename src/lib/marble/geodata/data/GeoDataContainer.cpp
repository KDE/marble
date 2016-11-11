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

// Marble
#include "MarbleDebug.h"
#include "GeoDataFeature.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataGeometry.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataNetworkLink.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataTour.h"

#include <QDataStream>

namespace Marble
{

GeoDataContainer::GeoDataContainer()
    : GeoDataFeature( new GeoDataContainerPrivate )
{
}

GeoDataContainer::GeoDataContainer(GeoDataContainerPrivate *priv)
    : GeoDataFeature(priv)
{
    Q_D(GeoDataContainer);
    d->setParent(this);
}

GeoDataContainer::GeoDataContainer(const GeoDataContainer& other, GeoDataContainerPrivate *priv)
    : GeoDataFeature(other, priv)
{
    Q_D(GeoDataContainer);
    d->setParent(this);
}

GeoDataContainer::GeoDataContainer( const GeoDataContainer& other )
    : GeoDataFeature(other, new GeoDataContainerPrivate(*other.d_func()))
{
    Q_D(GeoDataContainer);
    d->setParent(this);
}

GeoDataContainer::~GeoDataContainer()
{
}

GeoDataContainer& GeoDataContainer::operator=(const GeoDataContainer& other)
{
    if (this != &other) {
        Q_D(GeoDataContainer);
        *d = *other.d_func();
    }

    return *this;
}

bool GeoDataContainer::equals( const GeoDataContainer &other ) const
{
    if ( !GeoDataFeature::equals(other) ) {
        return false;
    }

    Q_D(const GeoDataContainer);
    const GeoDataContainerPrivate* const other_d = other.d_func();
    QVector<GeoDataFeature*>::const_iterator thisBegin = d->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator thisEnd = d->m_vector.constEnd();
    QVector<GeoDataFeature*>::const_iterator otherBegin = other_d->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator otherEnd = other_d->m_vector.constEnd();

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

const char* GeoDataContainer::nodeType() const
{
    return GeoDataTypes::GeoDataContainerType;
}


GeoDataFeature * GeoDataContainer::clone() const
{
    return new GeoDataContainer(*this);
}

GeoDataLatLonAltBox GeoDataContainer::latLonAltBox() const
{
    Q_D(const GeoDataContainer);
    GeoDataLatLonAltBox result;

    QVector<GeoDataFeature*>::const_iterator it = d->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator end = d->m_vector.constEnd();
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
    Q_D(const GeoDataContainer);
    QVector<GeoDataFolder*> results;

    QVector<GeoDataFeature*>::const_iterator it = d->m_vector.constBegin();
    QVector<GeoDataFeature*>::const_iterator end = d->m_vector.constEnd();

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
    Q_D(const GeoDataContainer);
    QVector<GeoDataPlacemark*> results;
    for (auto it = d->m_vector.constBegin(), end = d->m_vector.constEnd(); it != end; ++it) {
        if ((*it)->nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
            results.append(static_cast<GeoDataPlacemark*>(*it));
        }
    }
    return results;
}

QVector<GeoDataFeature*> GeoDataContainer::featureList() const
{
    Q_D(const GeoDataContainer);
    return d->m_vector;
}

/**
 * @brief  returns the requested child item
 */
GeoDataFeature* GeoDataContainer::child( int i )
{
    Q_D(GeoDataContainer);
    return d->m_vector.at(i);
}

const GeoDataFeature* GeoDataContainer::child( int i ) const
{
    Q_D(const GeoDataContainer);
    return d->m_vector.at(i);
}

/**
 * @brief returns the position of an item in the list
 */
int GeoDataContainer::childPosition( const GeoDataFeature* object ) const
{
    Q_D(const GeoDataContainer);
    for (int i = 0; i < d->m_vector.size(); ++i) {
        if (d->m_vector.at(i) == object) {
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
    Q_D(GeoDataContainer);
    feature->setParent(this);
    d->m_vector.insert( index, feature );
}

void GeoDataContainer::append( GeoDataFeature *other )
{
    Q_D(GeoDataContainer);
    other->setParent(this);
    d->m_vector.append( other );
}


void GeoDataContainer::remove( int index )
{
    Q_D(GeoDataContainer);
    d->m_vector.remove( index );
}

void GeoDataContainer::remove(int index, int count)
{
    Q_D(GeoDataContainer);
    d->m_vector.remove( index, count );
}

int	GeoDataContainer::removeAll(GeoDataFeature *feature)
{
    Q_D(GeoDataContainer);
#if QT_VERSION >= 0x050400
    return d->m_vector.removeAll(feature);
#else
    int count = 0;

    QVector<GeoDataFeature*> &vector = d->m_vector;
    QVector<GeoDataFeature*>::iterator it = vector.begin();

    while(it != vector.end()) {
        if (*it == feature) {
            it = vector.erase(it);
            ++count;
        } else {
            ++it;
        }
    }

    return count;
#endif
}

void GeoDataContainer::removeAt(int index)
{
    Q_D(GeoDataContainer);
    d->m_vector.removeAt( index );
}

void GeoDataContainer::removeFirst()
{
    Q_D(GeoDataContainer);
    d->m_vector.removeFirst();
}

void GeoDataContainer::removeLast()
{
    Q_D(GeoDataContainer);
    d->m_vector.removeLast();
}

bool GeoDataContainer::removeOne( GeoDataFeature *feature )
{
    Q_D(GeoDataContainer);
#if QT_VERSION >= 0x050400
    return d->m_vector.removeOne( feature );
#else
    QVector<GeoDataFeature*> &vector = d->m_vector;

    const int i = vector.indexOf(feature);
    if (i < 0) {
        return false;
    }

    vector.remove(i);

    return true;
#endif
}

int GeoDataContainer::size() const
{
    Q_D(const GeoDataContainer);
    return d->m_vector.size();
}

GeoDataFeature& GeoDataContainer::at( int pos )
{
    Q_D(GeoDataContainer);
    return *(d->m_vector[pos]);
}

const GeoDataFeature& GeoDataContainer::at( int pos ) const
{
    Q_D(const GeoDataContainer);
    return *(d->m_vector.at(pos));
}

GeoDataFeature& GeoDataContainer::last()
{
    Q_D(GeoDataContainer);
    return *(d->m_vector.last());
}

const GeoDataFeature& GeoDataContainer::last() const
{
    Q_D(const GeoDataContainer);
    return *(d->m_vector.last());
}

GeoDataFeature& GeoDataContainer::first()
{
    Q_D(GeoDataContainer);
    return *(d->m_vector.first());
}

const GeoDataFeature& GeoDataContainer::first() const
{
    Q_D(const GeoDataContainer);
    return *(d->m_vector.first());
}

void GeoDataContainer::clear()
{
    Q_D(GeoDataContainer);
    qDeleteAll(d->m_vector);
    d->m_vector.clear();
}

QVector<GeoDataFeature*>::Iterator GeoDataContainer::begin()
{
    Q_D(GeoDataContainer);
    return d->m_vector.begin();
}

QVector<GeoDataFeature*>::Iterator GeoDataContainer::end()
{
    Q_D(GeoDataContainer);
    return d->m_vector.end();
}

QVector<GeoDataFeature*>::ConstIterator GeoDataContainer::constBegin() const
{
    Q_D(const GeoDataContainer);
    return d->m_vector.constBegin();
}

QVector<GeoDataFeature*>::ConstIterator GeoDataContainer::constEnd() const
{
    Q_D(const GeoDataContainer);
    return d->m_vector.constEnd();
}

void GeoDataContainer::pack( QDataStream& stream ) const
{
    Q_D(const GeoDataContainer);
    GeoDataFeature::pack( stream );

    stream << d->m_vector.count();

    for (QVector<GeoDataFeature*>::const_iterator iterator = d->m_vector.constBegin();
         iterator != d->m_vector.constEnd();
          ++iterator )
    {
        const GeoDataFeature *feature = *iterator;
        stream << feature->featureId();
        feature->pack( stream );
    }
}

void GeoDataContainer::unpack( QDataStream& stream )
{
    Q_D(GeoDataContainer);
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
                d->m_vector.append( folder );
                }
                break;
            case GeoDataPlacemarkId:
                {
                GeoDataPlacemark *placemark = new GeoDataPlacemark;
                placemark->unpack( stream );
                d->m_vector.append( placemark );
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
