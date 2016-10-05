//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>
//


#include "GeoDataPolygon.h"
#include "GeoDataPolygon_p.h"

#include "MarbleDebug.h"

#include <QDataStream>


namespace Marble
{

GeoDataPolygon::GeoDataPolygon( TessellationFlags f )
    : GeoDataGeometry( new GeoDataPolygonPrivate( f ) )
{
    // nothing to do
}

GeoDataPolygon::GeoDataPolygon( const GeoDataGeometry & other )
    : GeoDataGeometry( other )
{
    // nothing to do
}

GeoDataPolygon::~GeoDataPolygon()
{
#ifdef DEBUG_GEODATA
    mDebug() << "delete polygon";
#endif
}

bool GeoDataPolygon::operator==( const GeoDataPolygon &other ) const
{
    Q_D(const GeoDataPolygon);
    const GeoDataPolygonPrivate *other_d = other.d_func();

    if ( !GeoDataGeometry::equals(other) ||
         tessellate() != other.tessellate() ||
         isClosed() != other.isClosed() ||
         d->inner.size() != other_d->inner.size() ||
         d->outer != other_d->outer ) {
        return false;
    }

    QVector<GeoDataLinearRing>::const_iterator itBound = d->inner.constBegin();
    QVector<GeoDataLinearRing>::const_iterator itEnd = d->inner.constEnd();
    QVector<GeoDataLinearRing>::const_iterator otherItBound = other_d->inner.constBegin();
    QVector<GeoDataLinearRing>::const_iterator otherItEnd= other_d->inner.constEnd();

    for ( ; itBound != itEnd && otherItBound != otherItEnd; ++itBound, ++otherItBound ) {
        if ( *itBound != *otherItBound) {
            return false;
        }
    }

    Q_ASSERT ( itBound == itEnd && otherItBound == otherItEnd );
    return true;
}

bool GeoDataPolygon::operator!=( const GeoDataPolygon &other ) const
{
    return !this->operator==(other);
}

bool GeoDataPolygon::isClosed() const
{
    return true;
}

bool GeoDataPolygon::tessellate() const
{
    Q_D(const GeoDataPolygon);
    return d->m_tessellationFlags.testFlag(Tessellate);
}

void GeoDataPolygon::setTessellate( bool tessellate )
{
    // According to the KML reference the tesselation is done along great circles
    // for polygons in Google Earth. Our "Tesselate" flag does this. 
    // Only for pure line strings and linear rings the 
    // latitude circles are followed for subsequent points that share the same latitude.
    detach();

    Q_D(GeoDataPolygon);
    if ( tessellate ) {
        d->m_tessellationFlags |= Tessellate;
    } else {
        d->m_tessellationFlags ^= Tessellate;
    }
}

TessellationFlags GeoDataPolygon::tessellationFlags() const
{
    Q_D(const GeoDataPolygon);
    return d->m_tessellationFlags;
}

void GeoDataPolygon::setTessellationFlags( TessellationFlags f )
{
    detach();

    Q_D(GeoDataPolygon);
    d->m_tessellationFlags = f;
}

const GeoDataLatLonAltBox& GeoDataPolygon::latLonAltBox() const
{
    Q_D(const GeoDataPolygon);
    return d->outer.latLonAltBox();
}

GeoDataLinearRing &GeoDataPolygon::outerBoundary()
{
    detach();

    Q_D(GeoDataPolygon);
    return (d->outer);
}

const GeoDataLinearRing &GeoDataPolygon::outerBoundary() const
{
    Q_D(const GeoDataPolygon);
    return d->outer;
}

void GeoDataPolygon::setOuterBoundary( const GeoDataLinearRing& boundary )
{
    detach();

    Q_D(GeoDataPolygon);
    d->outer = boundary;
}

QVector<GeoDataLinearRing>& GeoDataPolygon::innerBoundaries()
{
    detach();

    Q_D(GeoDataPolygon);
    return d->inner;
}

const QVector<GeoDataLinearRing>& GeoDataPolygon::innerBoundaries() const
{
    Q_D(const GeoDataPolygon);
    return d->inner;
}

void GeoDataPolygon::appendInnerBoundary( const GeoDataLinearRing& boundary )
{
    detach();

    Q_D(GeoDataPolygon);
    d->inner.append(boundary);
}

void GeoDataPolygon::setRenderOrder(int renderOrder)
{
    detach();

    Q_D(GeoDataPolygon);
    d->m_renderOrder = renderOrder;
}

int GeoDataPolygon::renderOrder() const
{
    Q_D(const GeoDataPolygon);
    return d->m_renderOrder;
}

void GeoDataPolygon::pack( QDataStream& stream ) const
{
    Q_D(const GeoDataPolygon);

    GeoDataObject::pack( stream );

    d->outer.pack( stream );

    stream << d->inner.size();
    stream << (qint32)(d->m_tessellationFlags);
   
    for( QVector<GeoDataLinearRing>::const_iterator iterator 
          = d->inner.constBegin();
         iterator != d->inner.constEnd();
         ++iterator ) {
        mDebug() << "innerRing: size" << d->inner.size();
        GeoDataLinearRing linearRing = ( *iterator );
        linearRing.pack( stream );
    }
}

void GeoDataPolygon::unpack( QDataStream& stream )
{
    detach();

    Q_D(GeoDataPolygon);

    GeoDataObject::unpack( stream );

    d->outer.unpack( stream );

    qint32 size;
    qint32 tessellationFlags;

    stream >> size;
    stream >> tessellationFlags;

    d->m_tessellationFlags = (TessellationFlags)(tessellationFlags);

    QVector<GeoDataLinearRing> &inner = d->inner;
    inner.reserve(inner.size() + size);
    for(qint32 i = 0; i < size; i++ ) {
        GeoDataLinearRing linearRing;
        linearRing.unpack( stream );
        inner.append(linearRing);
    }
}

bool GeoDataPolygon::contains( const GeoDataCoordinates &coordinates ) const
{
    if ( !outerBoundary().contains( coordinates ) ) {
        // Not inside the polygon at all
        return false;
    }

    foreach( const GeoDataLinearRing &ring, innerBoundaries() ) {
        if ( ring.contains( coordinates ) ) {
            // Inside the polygon, but in one of its holes
            return false;
        }
    }

    return true;
}

}
