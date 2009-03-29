//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataMultiGeometry.h"

#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"

#include <QtCore/QDebug>

#include "GeoDataMultiGeometry_p.h"

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

int GeoDataMultiGeometry::size() const
{
    return p()->m_vector.size();
}

QVector<GeoDataGeometry>& GeoDataMultiGeometry::vector() const
{
    return p()->m_vector;
}

GeoDataGeometry& GeoDataMultiGeometry::at( int pos )
{
    qDebug() << "detaching!";
    GeoDataGeometry::detach();
    return p()->m_vector[ pos ];
}

const GeoDataGeometry& GeoDataMultiGeometry::at( int pos ) const
{
    return p()->m_vector.at( pos );
}

GeoDataGeometry& GeoDataMultiGeometry::operator[]( int pos )
{
    GeoDataGeometry::detach();
    return p()->m_vector[ pos ];
}

const GeoDataGeometry& GeoDataMultiGeometry::operator[]( int pos ) const
{
    return p()->m_vector[ pos ];
}

GeoDataGeometry& GeoDataMultiGeometry::last()
{
    GeoDataGeometry::detach();
    return p()->m_vector.last();
}

GeoDataGeometry& GeoDataMultiGeometry::first()
{
    GeoDataGeometry::detach();
    return p()->m_vector.first();
}

const GeoDataGeometry& GeoDataMultiGeometry::last() const
{
    return p()->m_vector.last();
}

const GeoDataGeometry& GeoDataMultiGeometry::first() const
{
    return p()->m_vector.first();
}

QVector<GeoDataGeometry>::Iterator GeoDataMultiGeometry::begin()
{
    GeoDataGeometry::detach();
    return p()->m_vector.begin();
}

QVector<GeoDataGeometry>::Iterator GeoDataMultiGeometry::end()
{
    GeoDataGeometry::detach();
    return p()->m_vector.end();
}

QVector<GeoDataGeometry>::ConstIterator GeoDataMultiGeometry::constBegin() const
{
    return p()->m_vector.constBegin();
}

QVector<GeoDataGeometry>::ConstIterator GeoDataMultiGeometry::constEnd() const
{
    return p()->m_vector.constEnd();
}

void GeoDataMultiGeometry::append ( const GeoDataGeometry& value )
{
    GeoDataGeometry::detach();
    p()->m_vector.append( value );
}

GeoDataMultiGeometry& GeoDataMultiGeometry::operator << ( const GeoDataGeometry& value )
{
    GeoDataGeometry::detach();
    p()->m_vector.append( value );
    return *this;
}

void GeoDataMultiGeometry::clear()
{
    GeoDataGeometry::detach();
    p()->m_vector.clear();
}

void GeoDataMultiGeometry::pack( QDataStream& stream ) const
{
    GeoDataGeometry::pack( stream );

    stream << p()->m_vector.size();
    
    for( QVector<GeoDataGeometry>::const_iterator iterator 
          = p()->m_vector.constBegin(); 
         iterator != p()->m_vector.constEnd();
         ++iterator ) {
        const GeoDataGeometry& geometry = *iterator;
        stream << geometry.geometryId();
        geometry.pack( stream );
    }
}

void GeoDataMultiGeometry::unpack( QDataStream& stream )
{
    GeoDataGeometry::detach();
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
                GeoDataPoint point;
                point.unpack( stream );
                p()->m_vector.append( point );
                }
                break;
            case GeoDataLineStringId:
                {
                GeoDataLineString lineString;
                lineString.unpack( stream );
                p()->m_vector.append( lineString );
                }
                break;
            case GeoDataLinearRingId:
                {
                GeoDataLinearRing linearRing;
                linearRing.unpack( stream );
                p()->m_vector.append( linearRing );
                }
                break;
            case GeoDataPolygonId:
                {
                GeoDataPolygon polygon;
                polygon.unpack( stream );
                p()->m_vector.append( polygon );
                }
                break;
            case GeoDataMultiGeometryId:
                {
                GeoDataMultiGeometry multiGeometry;
                multiGeometry.unpack( stream );
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
