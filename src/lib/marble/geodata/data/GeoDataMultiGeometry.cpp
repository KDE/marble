//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataMultiGeometry.h"
#include "GeoDataMultiGeometry_p.h"

#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"

#include "MarbleDebug.h"

#include <QDataStream>


namespace Marble
{

GeoDataMultiGeometry::GeoDataMultiGeometry()
    : GeoDataGeometry( new GeoDataMultiGeometryPrivate )
{
}

GeoDataMultiGeometry::GeoDataMultiGeometry( const GeoDataGeometry& other )
    : GeoDataGeometry( other )
{
}

GeoDataMultiGeometry::~GeoDataMultiGeometry()
{
}

const GeoDataLatLonAltBox& GeoDataMultiGeometry::latLonAltBox() const
{
    Q_D(const GeoDataMultiGeometry);

    QVector<GeoDataGeometry*>::const_iterator it = d->m_vector.constBegin();
    QVector<GeoDataGeometry*>::const_iterator end = d->m_vector.constEnd();

    d->m_latLonAltBox.clear();
    for (; it != end; ++it) {
        if ( !(*it)->latLonAltBox().isEmpty() ) {
            if ( d->m_latLonAltBox.isEmpty() ) {
                d->m_latLonAltBox = (*it)->latLonAltBox();
            }
            else {
                d->m_latLonAltBox |= (*it)->latLonAltBox();
            }
        }
    }
    return d->m_latLonAltBox;
}

int GeoDataMultiGeometry::size() const
{
    Q_D(const GeoDataMultiGeometry);
    return d->m_vector.size();
}

QVector<GeoDataGeometry> GeoDataMultiGeometry::vector() const
{
    Q_D(const GeoDataMultiGeometry);
    QVector<GeoDataGeometry> results;

    QVector<GeoDataGeometry*>::const_iterator it = d->m_vector.constBegin();
    QVector<GeoDataGeometry*>::const_iterator end = d->m_vector.constEnd();

    for (; it != end; ++it) {
            GeoDataGeometry f = **it;
            results.append( f );
    }

    return results;
}

GeoDataGeometry& GeoDataMultiGeometry::at( int pos )
{
    mDebug() << "detaching!";
    detach();

    Q_D(GeoDataMultiGeometry);
    return *(d->m_vector[pos]);
}

const GeoDataGeometry& GeoDataMultiGeometry::at( int pos ) const
{
    Q_D(const GeoDataMultiGeometry);
    return *(d->m_vector.at(pos));
}

GeoDataGeometry& GeoDataMultiGeometry::operator[]( int pos )
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return *(d->m_vector[pos]);
}

const GeoDataGeometry& GeoDataMultiGeometry::operator[]( int pos ) const
{
    Q_D(const GeoDataMultiGeometry);
    return *(d->m_vector[pos]);
}

GeoDataGeometry& GeoDataMultiGeometry::last()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return *(d->m_vector.last());
}

GeoDataGeometry& GeoDataMultiGeometry::first()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return *(d->m_vector.first());
}

const GeoDataGeometry& GeoDataMultiGeometry::last() const
{
    Q_D(const GeoDataMultiGeometry);
    return *(d->m_vector.last());
}

const GeoDataGeometry& GeoDataMultiGeometry::first() const
{
    Q_D(const GeoDataMultiGeometry);
    return *(d->m_vector.first());
}

QVector<GeoDataGeometry*>::Iterator GeoDataMultiGeometry::begin()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return d->m_vector.begin();
}

QVector<GeoDataGeometry*>::Iterator GeoDataMultiGeometry::end()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return d->m_vector.end();
}

QVector<GeoDataGeometry*>::ConstIterator GeoDataMultiGeometry::constBegin() const
{
    Q_D(const GeoDataMultiGeometry);
    return d->m_vector.constBegin();
}

QVector<GeoDataGeometry*>::ConstIterator GeoDataMultiGeometry::constEnd() const
{
    Q_D(const GeoDataMultiGeometry);
    return d->m_vector.constEnd();
}

/**
 * @brief  returns the requested child item
 */
GeoDataGeometry* GeoDataMultiGeometry::child( int i )
{
    detach();

    Q_D(GeoDataMultiGeometry);
    return d->m_vector.at(i);
}

const GeoDataGeometry* GeoDataMultiGeometry::child( int i ) const
{
    Q_D(const GeoDataMultiGeometry);
    return d->m_vector.at(i);
}

/**
 * @brief returns the position of an item in the list
 */
int GeoDataMultiGeometry::childPosition( const GeoDataGeometry *object ) const
{
    Q_D(const GeoDataMultiGeometry);
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
void GeoDataMultiGeometry::append( GeoDataGeometry *other )
{
    detach();

    Q_D(GeoDataMultiGeometry);
    other->setParent( this );
    d->m_vector.append(other);
}


GeoDataMultiGeometry& GeoDataMultiGeometry::operator << ( const GeoDataGeometry& value )
{
    detach();

    Q_D(GeoDataMultiGeometry);
    GeoDataGeometry *g = new GeoDataGeometry( value );
    g->setParent( this );
    d->m_vector.append(g);
    return *this;
}

void GeoDataMultiGeometry::clear()
{
    detach();

    Q_D(GeoDataMultiGeometry);
    qDeleteAll(d->m_vector);
    d->m_vector.clear();
}

void GeoDataMultiGeometry::pack( QDataStream& stream ) const
{
    Q_D(const GeoDataMultiGeometry);

    GeoDataGeometry::pack( stream );

    stream << d->m_vector.size();

    for( QVector<GeoDataGeometry*>::const_iterator iterator
          = d->m_vector.constBegin();
         iterator != d->m_vector.constEnd();
         ++iterator ) {
        const GeoDataGeometry *geometry = *iterator;
        stream << geometry->geometryId();
        geometry->pack( stream );
    }
}

void GeoDataMultiGeometry::unpack( QDataStream& stream )
{
    detach();

    Q_D(GeoDataMultiGeometry);
    GeoDataGeometry::unpack( stream );

    int size = 0;
    
    stream >> size;
    
    for( int i = 0; i < size; i++ ) {
        int geometryId;
        stream >> geometryId;
        switch( geometryId ) {
            case InvalidGeometryId:
                break;
            case GeoDataPointId:
                {
                GeoDataPoint *point = new GeoDataPoint;
                point->unpack( stream );
                d->m_vector.append(point);
                }
                break;
            case GeoDataLineStringId:
                {
                GeoDataLineString *lineString = new GeoDataLineString;
                lineString->unpack( stream );
                d->m_vector.append(lineString);
                }
                break;
            case GeoDataLinearRingId:
                {
                GeoDataLinearRing *linearRing = new GeoDataLinearRing;
                linearRing->unpack( stream );
                d->m_vector.append(linearRing);
                }
                break;
            case GeoDataPolygonId:
                {
                GeoDataPolygon *polygon = new GeoDataPolygon;
                polygon->unpack( stream );
                d->m_vector.append(polygon);
                }
                break;
            case GeoDataMultiGeometryId:
                {
                GeoDataMultiGeometry *multiGeometry = new GeoDataMultiGeometry;
                multiGeometry->unpack( stream );
                d->m_vector.append(multiGeometry);
                }
                break;
            case GeoDataModelId:
                break;
            default: break;
        };
    }
}

}
