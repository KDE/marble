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

GeoDataMultiGeometryPrivate* GeoDataMultiGeometry::p() const
{
    return static_cast<GeoDataMultiGeometryPrivate*>(d);
}

GeoDataLatLonAltBox& GeoDataMultiGeometry::latLonAltBox() const
{
    QVector<GeoDataGeometry*>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataGeometry*>::const_iterator end = p()->m_vector.constEnd();

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

int GeoDataMultiGeometry::size() const
{
    return p()->m_vector.size();
}

QVector<GeoDataGeometry> GeoDataMultiGeometry::vector() const
{
    QVector<GeoDataGeometry> results;

    QVector<GeoDataGeometry*>::const_iterator it = p()->m_vector.constBegin();
    QVector<GeoDataGeometry*>::const_iterator end = p()->m_vector.constEnd();

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
    return *(p()->m_vector[ pos ]);
}

const GeoDataGeometry& GeoDataMultiGeometry::at( int pos ) const
{
    return *(p()->m_vector.at( pos ));
}

GeoDataGeometry& GeoDataMultiGeometry::operator[]( int pos )
{
    detach();
    return *(p()->m_vector[ pos ]);
}

const GeoDataGeometry& GeoDataMultiGeometry::operator[]( int pos ) const
{
    return *(p()->m_vector[ pos ]);
}

GeoDataGeometry& GeoDataMultiGeometry::last()
{
    detach();
    return *(p()->m_vector.last());
}

GeoDataGeometry& GeoDataMultiGeometry::first()
{
    detach();
    return *(p()->m_vector.first());
}

const GeoDataGeometry& GeoDataMultiGeometry::last() const
{
    return *(p()->m_vector.last());
}

const GeoDataGeometry& GeoDataMultiGeometry::first() const
{
    return *(p()->m_vector.first());
}

QVector<GeoDataGeometry*>::Iterator GeoDataMultiGeometry::begin()
{
    detach();
    return p()->m_vector.begin();
}

QVector<GeoDataGeometry*>::Iterator GeoDataMultiGeometry::end()
{
    detach();
    return p()->m_vector.end();
}

QVector<GeoDataGeometry*>::ConstIterator GeoDataMultiGeometry::constBegin() const
{
    return p()->m_vector.constBegin();
}

QVector<GeoDataGeometry*>::ConstIterator GeoDataMultiGeometry::constEnd() const
{
    return p()->m_vector.constEnd();
}

/**
 * @brief  returns the requested child item
 */
GeoDataGeometry* GeoDataMultiGeometry::child( int i )
{
    return p()->m_vector.at( i );
}

const GeoDataGeometry* GeoDataMultiGeometry::child( int i ) const
{
    return p()->m_vector.at( i );
}

/**
 * @brief returns the position of an item in the list
 */
int GeoDataMultiGeometry::childPosition( GeoDataGeometry *object)
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
void GeoDataMultiGeometry::append( GeoDataGeometry *other )
{
    detach();
    other->setParent( this );
    p()->m_vector.append( other );
}


GeoDataMultiGeometry& GeoDataMultiGeometry::operator << ( const GeoDataGeometry& value )
{
    detach();
    GeoDataGeometry *g = new GeoDataGeometry( value );
    g->setParent( this );
    p()->m_vector.append( g );
    return *this;
}

void GeoDataMultiGeometry::clear()
{
    detach();
    qDeleteAll(p()->m_vector);
    p()->m_vector.clear();
}

void GeoDataMultiGeometry::pack( QDataStream& stream ) const
{
    GeoDataGeometry::pack( stream );

    stream << p()->m_vector.size();
    
    for( QVector<GeoDataGeometry*>::const_iterator iterator
          = p()->m_vector.constBegin(); 
         iterator != p()->m_vector.constEnd();
         ++iterator ) {
        const GeoDataGeometry *geometry = *iterator;
        stream << geometry->geometryId();
        geometry->pack( stream );
    }
}

void GeoDataMultiGeometry::unpack( QDataStream& stream )
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
            case GeoDataPointId:
                {
                GeoDataPoint *point = new GeoDataPoint;
                point->unpack( stream );
                p()->m_vector.append( point );
                }
                break;
            case GeoDataLineStringId:
                {
                GeoDataLineString *lineString = new GeoDataLineString;
                lineString->unpack( stream );
                p()->m_vector.append( lineString );
                }
                break;
            case GeoDataLinearRingId:
                {
                GeoDataLinearRing *linearRing = new GeoDataLinearRing;
                linearRing->unpack( stream );
                p()->m_vector.append( linearRing );
                }
                break;
            case GeoDataPolygonId:
                {
                GeoDataPolygon *polygon = new GeoDataPolygon;
                polygon->unpack( stream );
                p()->m_vector.append( polygon );
                }
                break;
            case GeoDataMultiGeometryId:
                {
                GeoDataMultiGeometry *multiGeometry = new GeoDataMultiGeometry;
                multiGeometry->unpack( stream );
                p()->m_vector.append( multiGeometry );
                }
                break;
            case GeoDataModelId:
                break;
            default: break;
        };
    }
}

}
