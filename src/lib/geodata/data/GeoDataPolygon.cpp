//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#include "GeoDataPolygon.h"

#include <QtCore/QDebug>


class GeoDataPolygonPrivate
{
 public:
    GeoDataPolygonPrivate()
         : m_dirtyBox( true ),
           outer( new GeoDataLinearRing() )
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

GeoDataPolygon::GeoDataPolygon()
  : GeoDataGeometry(),
    d_polyg( new GeoDataPolygonPrivate() )
{
}

GeoDataPolygon::GeoDataPolygon( const GeoDataPolygon & other )
  : GeoDataGeometry( other ),
    d_polyg( new GeoDataPolygonPrivate( *other.d_polyg ) )
{
}

GeoDataPolygon& GeoDataPolygon::operator=( const GeoDataPolygon & other )
{
    *d_polyg = *other.d_polyg;
    return *this;
}

GeoDataPolygon::~GeoDataPolygon()
{
#if DEBUG_GEODATA
    qDebug() << "delete polygon";
#endif
    delete d_polyg;
}

GeoDataLatLonAltBox GeoDataPolygon::latLonAltBox() const
{
    if (d_polyg->m_dirtyBox) {
    // calulate LatLonAltBox
    }
    d_polyg->m_dirtyBox = false;

    return GeoDataLatLonAltBox();
}

GeoDataLinearRing& GeoDataPolygon::outerBoundary() const
{
    return *(d_polyg->outer);
}

void GeoDataPolygon::setOuterBoundary( GeoDataLinearRing* boundary )
{
    delete d_polyg->outer;
    d_polyg->outer = boundary;
}

QVector<GeoDataLinearRing*> GeoDataPolygon::innerBoundaries() const
{
    return d_polyg->inner;
}

void GeoDataPolygon::appendInnerBoundary( GeoDataLinearRing* boundary )
{
    d_polyg->inner.append( boundary );
}

void GeoDataPolygon::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    d_polyg->outer->pack( stream );
    
    stream << d_polyg->inner.size();
    
    for( QVector<GeoDataLinearRing*>::const_iterator iterator 
          = d_polyg->inner.constBegin(); 
         iterator != d_polyg->inner.constEnd();
         ++iterator ) {
        qDebug() << "innerRing: size" << d_polyg->inner.size();
        GeoDataLinearRing* linearRing = ( *iterator );
        linearRing->pack( stream );
    }
}

void GeoDataPolygon::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    d_polyg->outer->unpack( stream );
    int size;
    
    stream >> size;
    for(int i = 0; i < size; i++ ) {
        GeoDataLinearRing* linearRing = new GeoDataLinearRing();
        linearRing->unpack( stream );
        d_polyg->inner.append( linearRing );
    }
}
