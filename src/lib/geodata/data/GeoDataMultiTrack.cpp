//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Thibaut Gridel <tgridel@free.fr>

#include "GeoDataMultiTrack.h"
#include "GeoDataMultiTrack_p.h"

#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataTrack.h"

#include "MarbleDebug.h"


namespace Marble
{

GeoDataMultiTrack::GeoDataMultiTrack()
    : GeoDataGeometry( new GeoDataMultiTrackPrivate )
{
}

GeoDataMultiTrack::GeoDataMultiTrack( const GeoDataGeometry& other )
    : GeoDataGeometry( other )
{
}

GeoDataMultiTrack::~GeoDataMultiTrack()
{
}

GeoDataMultiTrackPrivate* GeoDataMultiTrack::p() const
{
    return static_cast<GeoDataMultiTrackPrivate*>(d);
}


GeoDataLatLonAltBox& GeoDataMultiTrack::latLonAltBox() const
{
    QVector<GeoDataTrack*>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataTrack*>::const_iterator end = p()->m_vector.constEnd();

    p()->m_latLonAltBox.clear();
    for (; it != end; ++it) {
        if ( !(*it)->latLonAltBox().isEmpty() ) {
            if ( p()->m_latLonAltBox.isEmpty() ) {
                p()->m_latLonAltBox = (*it)->latLonAltBox();
            }
            else {
                p()->m_latLonAltBox |= (*it)->latLonAltBox();
            }
        }
    }
    return p()->m_latLonAltBox;
}

int GeoDataMultiTrack::size() const
{
    return p()->m_vector.size();
}

QVector<GeoDataTrack> GeoDataMultiTrack::vector() const
{
    QVector<GeoDataTrack> results;

    QVector<GeoDataTrack*>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataTrack*>::const_iterator end = p()->m_vector.constEnd();

    for (; it != end; ++it) {
            GeoDataTrack f = **it;
            results.append( f );
    }

    return results;
}

GeoDataTrack& GeoDataMultiTrack::at( int pos )
{
    mDebug() << "detaching!";
    detach();
    return *(p()->m_vector[ pos ]);
}

const GeoDataTrack& GeoDataMultiTrack::at( int pos ) const
{
    return *(p()->m_vector.at( pos ));
}

GeoDataTrack& GeoDataMultiTrack::operator[]( int pos )
{
    detach();
    return *(p()->m_vector[ pos ]);
}

const GeoDataTrack& GeoDataMultiTrack::operator[]( int pos ) const
{
    return *(p()->m_vector[ pos ]);
}

GeoDataTrack& GeoDataMultiTrack::last()
{
    detach();
    return *(p()->m_vector.last());
}

GeoDataTrack& GeoDataMultiTrack::first()
{
    detach();
    return *(p()->m_vector.first());
}

const GeoDataTrack& GeoDataMultiTrack::last() const
{
    return *(p()->m_vector.last());
}

const GeoDataTrack& GeoDataMultiTrack::first() const
{
    return *(p()->m_vector.first());
}

QVector<GeoDataTrack*>::Iterator GeoDataMultiTrack::begin()
{
    detach();
    return p()->m_vector.begin();
}

QVector<GeoDataTrack*>::Iterator GeoDataMultiTrack::end()
{
    detach();
    return p()->m_vector.end();
}

QVector<GeoDataTrack*>::ConstIterator GeoDataMultiTrack::constBegin() const
{
    return p()->m_vector.constBegin();
}

QVector<GeoDataTrack*>::ConstIterator GeoDataMultiTrack::constEnd() const
{
    return p()->m_vector.constEnd();
}

/**
 * @brief  returns the requested child item
 */
GeoDataTrack* GeoDataMultiTrack::child( int i )
{
    return p()->m_vector.at( i );
}

const GeoDataTrack* GeoDataMultiTrack::child( int i ) const
{
    return p()->m_vector.at( i );
}

/**
 * @brief returns the position of an item in the list
 */
int GeoDataMultiTrack::childPosition( GeoDataTrack *object)
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

/**
* @brief add an element
*/
void GeoDataMultiTrack::append( GeoDataTrack *other )
{
    detach();
    other->setParent( this );
    p()->m_vector.append( other );
}


GeoDataMultiTrack& GeoDataMultiTrack::operator << ( const GeoDataTrack& value )
{
    detach();
    GeoDataTrack *g = new GeoDataTrack( value );
    g->setParent( this );
    p()->m_vector.append( g );
    return *this;
}

void GeoDataMultiTrack::clear()
{
    detach();
    qDeleteAll(p()->m_vector);
    p()->m_vector.clear();
}

void GeoDataMultiTrack::pack( QDataStream& stream ) const
{
    GeoDataGeometry::pack( stream );

    stream << p()->m_vector.size();

    for( QVector<GeoDataTrack*>::const_iterator iterator
          = p()->m_vector.constBegin();
         iterator != p()->m_vector.constEnd();
         ++iterator ) {
        const GeoDataTrack *geometry = *iterator;
        stream << geometry->geometryId();
        geometry->pack( stream );
    }
}

void GeoDataMultiTrack::unpack( QDataStream& stream )
{
    detach();
    GeoDataGeometry::unpack( stream );

    int size = 0;

    stream >> size;

    for( int i = 0; i < size; i++ ) {
        int geometryId;
        stream >> geometryId;
        switch( geometryId ) {
            case InvalidGeometryId:
                break;
            case GeoDataTrackId:
                {
                GeoDataTrack *track = new GeoDataTrack;
                track->unpack( stream );
                p()->m_vector.append( track );
                }
                break;
            case GeoDataModelId:
                break;
            default: break;
        };
    }
}

}
