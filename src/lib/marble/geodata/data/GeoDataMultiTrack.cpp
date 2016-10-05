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

#include <QDataStream>


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

bool GeoDataMultiTrack::operator==( const GeoDataMultiTrack& other ) const
{
    if ( !equals(other) ) return false;

    Q_D(const GeoDataMultiTrack);
    const GeoDataMultiTrackPrivate *other_d = other.d_func();

    QVector<GeoDataTrack*>::const_iterator d_it = d->m_vector.constBegin();
    QVector<GeoDataTrack*>::const_iterator d_end = d->m_vector.constEnd();
    QVector<GeoDataTrack*>::const_iterator other_it = other_d->m_vector.constBegin();
    QVector<GeoDataTrack*>::const_iterator other_end = other_d->m_vector.constEnd();


    for (; d_it != d_end && other_it != other_end; ++d_it, ++other_it) {
        if ( **d_it != **other_it ) return false;
    }

    return d_it == d_end && other_it == other_end;
}

bool GeoDataMultiTrack::operator!=( const GeoDataMultiTrack& other ) const
{
    return !this->operator==( other );
}

const GeoDataLatLonAltBox& GeoDataMultiTrack::latLonAltBox() const
{
    Q_D(const GeoDataMultiTrack);

    QVector<GeoDataTrack*>::const_iterator it = d->m_vector.constBegin();
    QVector<GeoDataTrack*>::const_iterator end = d->m_vector.constEnd();

    d->m_latLonAltBox.clear();
    for (; it != end; ++it) {
        if ( !(*it)->latLonAltBox().isEmpty() ) {
            if (d->m_latLonAltBox.isEmpty() ) {
                d->m_latLonAltBox = (*it)->latLonAltBox();
            }
            else {
                d->m_latLonAltBox |= (*it)->latLonAltBox();
            }
        }
    }
    return d->m_latLonAltBox;
}

int GeoDataMultiTrack::size() const
{
    Q_D(const GeoDataMultiTrack);
    return d->m_vector.size();
}

QVector<GeoDataTrack> GeoDataMultiTrack::vector() const
{
    Q_D(const GeoDataMultiTrack);

    QVector<GeoDataTrack> results;
    results.reserve(d->m_vector.size());

    QVector<GeoDataTrack*>::const_iterator it = d->m_vector.constBegin();
    QVector<GeoDataTrack*>::const_iterator end = d->m_vector.constEnd();

    for (; it != end; ++it) {
            const GeoDataTrack f(**it);
            results.append( f );
    }

    return results;
}

GeoDataTrack& GeoDataMultiTrack::at( int pos )
{
    mDebug() << "detaching!";
    detach();

    Q_D(GeoDataMultiTrack);
    return *(d->m_vector[pos]);
}

const GeoDataTrack& GeoDataMultiTrack::at( int pos ) const
{
    Q_D(const GeoDataMultiTrack);
    return *(d->m_vector.at(pos));
}

GeoDataTrack& GeoDataMultiTrack::operator[]( int pos )
{
    detach();

    Q_D(GeoDataMultiTrack);
    return *(d->m_vector[pos]);
}

const GeoDataTrack& GeoDataMultiTrack::operator[]( int pos ) const
{
    Q_D(const GeoDataMultiTrack);
    return *(d->m_vector[pos]);
}

GeoDataTrack& GeoDataMultiTrack::last()
{
    detach();

    Q_D(GeoDataMultiTrack);
    return *(d->m_vector.last());
}

GeoDataTrack& GeoDataMultiTrack::first()
{
    detach();

    Q_D(GeoDataMultiTrack);
    return *(d->m_vector.first());
}

const GeoDataTrack& GeoDataMultiTrack::last() const
{
    Q_D(const GeoDataMultiTrack);
    return *(d->m_vector.last());
}

const GeoDataTrack& GeoDataMultiTrack::first() const
{
    Q_D(const GeoDataMultiTrack);
    return *(d->m_vector.first());
}

QVector<GeoDataTrack*>::Iterator GeoDataMultiTrack::begin()
{
    detach();

    Q_D(GeoDataMultiTrack);
    return d->m_vector.begin();
}

QVector<GeoDataTrack*>::Iterator GeoDataMultiTrack::end()
{
    detach();

    Q_D(GeoDataMultiTrack);
    return d->m_vector.end();
}

QVector<GeoDataTrack*>::ConstIterator GeoDataMultiTrack::constBegin() const
{
    Q_D(const GeoDataMultiTrack);
    return d->m_vector.constBegin();
}

QVector<GeoDataTrack*>::ConstIterator GeoDataMultiTrack::constEnd() const
{
    Q_D(const GeoDataMultiTrack);
    return d->m_vector.constEnd();
}

/**
 * @brief  returns the requested child item
 */
GeoDataTrack* GeoDataMultiTrack::child( int i )
{
    detach();

    Q_D(const GeoDataMultiTrack);
    return d->m_vector.at(i);
}

const GeoDataTrack* GeoDataMultiTrack::child( int i ) const
{
    Q_D(const GeoDataMultiTrack);
    return d->m_vector.at(i);
}

/**
 * @brief returns the position of an item in the list
 */
int GeoDataMultiTrack::childPosition( const GeoDataTrack *object ) const
{
    Q_D(const GeoDataMultiTrack);
    for (int i = 0; i < d->m_vector.size(); ++i) {
        if (d->m_vector.at(i) == object) {
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

    Q_D(GeoDataMultiTrack);
    other->setParent( this );
    d->m_vector.append(other);
}


GeoDataMultiTrack& GeoDataMultiTrack::operator << ( const GeoDataTrack& value )
{
    detach();

    Q_D(GeoDataMultiTrack);
    GeoDataTrack *g = new GeoDataTrack( value );
    g->setParent( this );
    d->m_vector.append(g);
    return *this;
}

void GeoDataMultiTrack::clear()
{
    detach();

    Q_D(GeoDataMultiTrack);
    qDeleteAll(d->m_vector);
    d->m_vector.clear();
}

void GeoDataMultiTrack::pack( QDataStream& stream ) const
{
    Q_D(const GeoDataMultiTrack);

    GeoDataGeometry::pack( stream );

    stream << d->m_vector.size();

    for( QVector<GeoDataTrack*>::const_iterator iterator
          = d->m_vector.constBegin();
         iterator != d->m_vector.constEnd();
         ++iterator ) {
        const GeoDataTrack *geometry = *iterator;
        stream << geometry->geometryId();
        geometry->pack( stream );
    }
}

void GeoDataMultiTrack::unpack( QDataStream& stream )
{
    detach();

    Q_D(GeoDataMultiTrack);
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
                d->m_vector.append( track );
                }
                break;
            case GeoDataModelId:
                break;
            default: break;
        };
    }
}

}
