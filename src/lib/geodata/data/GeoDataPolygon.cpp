//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>
//


#include "GeoDataPolygon.h"

#include <QtCore/QDebug>

namespace Marble
{

class GeoDataPolygonPrivate
{
 public:
    GeoDataPolygonPrivate( GeoDataPolygon *geoDataPolygon )
	: outer( new GeoDataLinearRing( geoDataPolygon ) ),
	  m_dirtyBox( true )
    {
    }
    
    ~GeoDataPolygonPrivate()
    {
//        qDeleteAll( *outer );
        delete outer;
        
        // LinearRings are equiv. to QVector<GeoDataCoordinates>
        // thus the next line hopefully deletes all GeoDataCoordinates
        // in this QVector
        qDeleteAll(inner);
    }
    
    GeoDataLinearRing*          outer;
    QVector<GeoDataLinearRing*> inner;
    bool                        m_dirtyBox; // tells whether there have been changes to the
                                            // GeoDataPoints since the LatLonAltBox has 
                                            // been calculated. Saves performance. 
};

GeoDataPolygon::GeoDataPolygon( GeoDataObject *parent )
  : GeoDataGeometry( parent ),
    d( new GeoDataPolygonPrivate( this ) )
{
}

GeoDataPolygon::GeoDataPolygon( const GeoDataPolygon & other )
  : GeoDataGeometry( other ),
    d( new GeoDataPolygonPrivate( *other.d ) )
{
}

GeoDataPolygon& GeoDataPolygon::operator=( const GeoDataPolygon & other )
{
    *d = *other.d;
    return *this;
}

GeoDataPolygon::~GeoDataPolygon()
{
#ifdef DEBUG_GEODATA
    qDebug() << "delete polygon";
#endif
    delete d;
}

GeoDataLatLonAltBox GeoDataPolygon::latLonAltBox() const
{
    if (d->m_dirtyBox) {
    // calulate LatLonAltBox
    }
    d->m_dirtyBox = false;

    return GeoDataLatLonAltBox();
}

GeoDataLinearRing& GeoDataPolygon::outerBoundary() const
{
    return *(d->outer);
}

void GeoDataPolygon::setOuterBoundary( GeoDataLinearRing* boundary )
{
    delete d->outer;
    d->outer = boundary;
}

QVector<GeoDataLinearRing*> GeoDataPolygon::innerBoundaries() const
{
    return d->inner;
}

void GeoDataPolygon::appendInnerBoundary( GeoDataLinearRing* boundary )
{
    d->inner.append( boundary );
}

void GeoDataPolygon::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    d->outer->pack( stream );
    
    stream << d->inner.size();
    
    for( QVector<GeoDataLinearRing*>::const_iterator iterator 
          = d->inner.constBegin(); 
         iterator != d->inner.constEnd();
         ++iterator ) {
        qDebug() << "innerRing: size" << d->inner.size();
        GeoDataLinearRing* linearRing = ( *iterator );
        linearRing->pack( stream );
    }
}

void GeoDataPolygon::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    d->outer->unpack( stream );
    int size;
    
    stream >> size;
    for(int i = 0; i < size; i++ ) {
        GeoDataLinearRing* linearRing = new GeoDataLinearRing( this );
        linearRing->unpack( stream );
        d->inner.append( linearRing );
    }
}

}
