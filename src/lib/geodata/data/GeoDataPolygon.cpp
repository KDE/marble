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


namespace Marble
{

GeoDataPolygon::GeoDataPolygon( TessellationFlags f )
  : GeoDataGeometry( new GeoDataPolygonPrivate( f ) )
{
}

GeoDataPolygon::GeoDataPolygon( const GeoDataGeometry & other )
  : GeoDataGeometry( other )
{
}

GeoDataPolygon::~GeoDataPolygon()
{
#ifdef DEBUG_GEODATA
    mDebug() << "delete polygon";
#endif
}

GeoDataPolygonPrivate* GeoDataPolygon::p() const
{
    return static_cast<GeoDataPolygonPrivate*>(d);
}

bool GeoDataPolygon::isClosed() const
{
    return true;
}

bool GeoDataPolygon::tessellate() const
{
    return p()->m_tessellationFlags.testFlag(Tessellate);
}

void GeoDataPolygon::setTessellate( bool tessellate )
{
    // According to the KML reference the tesselation is done along great circles
    // for polygons in Google Earth. Our "Tesselate" flag does this. 
    // Only for pure line strings and linear rings the 
    // latitude circles are followed for subsequent points that share the same latitude.
    GeoDataGeometry::detach();

    if ( tessellate ) {
        p()->m_tessellationFlags |= Tessellate; 
    } else {
        p()->m_tessellationFlags ^= Tessellate; 
    }
}

TessellationFlags GeoDataPolygon::tessellationFlags() const
{
    return p()->m_tessellationFlags;
}

void GeoDataPolygon::setTessellationFlags( TessellationFlags f )
{
    GeoDataGeometry::detach();
    p()->m_tessellationFlags = f;
}

GeoDataLatLonAltBox& GeoDataPolygon::latLonAltBox() const
{
    return p()->outer.latLonAltBox();
}

GeoDataLinearRing& GeoDataPolygon::outerBoundary() const
{
    return (p()->outer);
}

void GeoDataPolygon::setOuterBoundary( const GeoDataLinearRing& boundary )
{
    GeoDataGeometry::detach();
    p()->outer = boundary;
}

QVector<GeoDataLinearRing>& GeoDataPolygon::innerBoundaries() const
{
    return p()->inner;
}

void GeoDataPolygon::appendInnerBoundary( const GeoDataLinearRing& boundary )
{
    GeoDataGeometry::detach();
    p()->inner.append( boundary );
}

void GeoDataPolygon::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    p()->outer.pack( stream );
    
    stream << p()->inner.size();
    stream << (qint32)(p()->m_tessellationFlags);
   
    for( QVector<GeoDataLinearRing>::const_iterator iterator 
          = p()->inner.constBegin(); 
         iterator != p()->inner.constEnd();
         ++iterator ) {
        mDebug() << "innerRing: size" << p()->inner.size();
        GeoDataLinearRing linearRing = ( *iterator );
        linearRing.pack( stream );
    }
}

void GeoDataPolygon::unpack( QDataStream& stream )
{
    GeoDataGeometry::detach();
    GeoDataObject::unpack( stream );

    p()->outer.unpack( stream );

    qint32 size;
    qint32 tessellationFlags;

    stream >> size;
    stream >> tessellationFlags;

    p()->m_tessellationFlags = (TessellationFlags)(tessellationFlags);

    for(qint32 i = 0; i < size; i++ ) {
        GeoDataLinearRing linearRing;
        linearRing.unpack( stream );
        p()->inner.append( linearRing );
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
