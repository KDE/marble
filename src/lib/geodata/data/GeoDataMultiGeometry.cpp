//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#include "GeoDataMultiGeometry.h"

#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"

GeoDataMultiGeometry::GeoDataMultiGeometry()
{
}

GeoDataMultiGeometry::GeoDataMultiGeometry( const GeoDataMultiGeometry& other )
        : QVector<GeoDataGeometry*>( other ),
          GeoDataGeometry( other )
{
}

GeoDataMultiGeometry::~GeoDataMultiGeometry()
{
    qDeleteAll( *this );
}

void GeoDataMultiGeometry::pack( QDataStream& stream ) const
{
    GeoDataGeometry::pack( stream );

    stream << QVector<GeoDataGeometry*>::size();
    
    for( QVector<GeoDataGeometry*>::const_iterator iterator 
          = this->constBegin(); 
         iterator != this->constEnd();
         ++iterator ) {
        const GeoDataGeometry& geometry = * ( *iterator );
        stream << geometry.geometryId();
        geometry.pack( stream );
    }
}

void GeoDataMultiGeometry::unpack( QDataStream& stream )
{
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
                GeoDataPoint* point = new GeoDataPoint();
                point->unpack( stream );
                this->append( point );
                }
                break;
            case GeoDataLineStringId:
                {
                GeoDataLineString* lineString = new GeoDataLineString();
                lineString->unpack( stream );
                this->append( lineString );
                }
                break;
            case GeoDataLinearRingId:
                {
                GeoDataLinearRing* linearRing = new GeoDataLinearRing();
                linearRing->unpack( stream );
                this->append( linearRing );
                }
                break;
            case GeoDataPolygonId:
                {
                GeoDataPolygon* polygon = new GeoDataPolygon();
                polygon->unpack( stream );
                this->append( polygon );
                }
                break;
            case GeoDataMultiGeometryId:
                {
                GeoDataMultiGeometry* multiGeometry = new GeoDataMultiGeometry();
                multiGeometry->unpack( stream );
                this->append( multiGeometry );
                }
                break;
            case GeoDataModelId:
                break;
            default: break;
        };
    }
}
