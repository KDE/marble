//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


// Own
#include "GeoDataPlacemark.h"

// Qt
#include <QtCore/QDataStream>
#include <QtCore/QDebug>

namespace Marble
{

class GeoDataPlacemarkPrivate
{
  public:
    GeoDataPlacemarkPrivate()
      : m_geometry( 0 ),
	m_area( -1.0 ),
	m_population( -1 )
    {
    }
    
    GeoDataPlacemarkPrivate( const GeoDataPlacemarkPrivate& other )
     : m_coordinate( other.m_coordinate ),
       m_countrycode( other.m_countrycode ),
       m_area( other.m_area ),
       m_population( other.m_population ),
       m_geometry( 0 )
    {
        if( !other.m_geometry ) return;

        switch( other.m_geometry->geometryId() ) {
            case InvalidGeometryId:
                break;
            case GeoDataPointId:
                m_geometry = new GeoDataPoint( *static_cast<GeoDataPoint*>( other.m_geometry ) );
                break;
            case GeoDataLineStringId:
                m_geometry = new GeoDataLineString( *static_cast<GeoDataLineString*>( other.m_geometry ) );
                break;
            case GeoDataLinearRingId:
                m_geometry = new GeoDataLinearRing( *static_cast<GeoDataLinearRing*>( other.m_geometry ) );
                break;
            case GeoDataPolygonId:
                m_geometry = new GeoDataPolygon( *static_cast<GeoDataPolygon*>( other.m_geometry ) );
                break;
            case GeoDataMultiGeometryId:
                m_geometry = new GeoDataMultiGeometry( *static_cast<GeoDataMultiGeometry*>( other.m_geometry ) );
                break;
            case GeoDataModelId:
                break;
            default: break;
        };
    }
    
    void operator=( const GeoDataPlacemarkPrivate& other )
    {
        m_coordinate = other.m_coordinate;
        m_countrycode = other.m_countrycode;
        m_area = other.m_area;
        m_population = other.m_population;

        delete m_geometry;
        if( !other.m_geometry ) return;

        switch( other.m_geometry->geometryId() ) {
            case InvalidGeometryId:
                break;
            case GeoDataPointId:
                m_geometry = new GeoDataPoint( *static_cast<GeoDataPoint*>( other.m_geometry ) );
                break;
            case GeoDataLineStringId:
                m_geometry = new GeoDataLineString( *static_cast<GeoDataLineString*>( other.m_geometry ) );
                break;
            case GeoDataLinearRingId:
                m_geometry = new GeoDataLinearRing( *static_cast<GeoDataLinearRing*>( other.m_geometry ) );
                break;
            case GeoDataPolygonId:
                m_geometry = new GeoDataPolygon( *static_cast<GeoDataPolygon*>( other.m_geometry ) );
                break;
            case GeoDataMultiGeometryId:
                m_geometry = new GeoDataMultiGeometry( *static_cast<GeoDataMultiGeometry*>( other.m_geometry ) );
                break;
            case GeoDataModelId:
                break;
            default: break;
        };
    }

    ~GeoDataPlacemarkPrivate()
    {
    }

    // Data for a Placemark in addition to those in GeoDataFeature.
    GeoDataGeometry    *m_geometry;     // any GeoDataGeometry entry like locations
    GeoDataPoint        m_coordinate;     // The geographic position
    QString             m_countrycode;  // Country code.
    qreal               m_area;         // Area in square kilometer
    qint64              m_population;   // population in number of inhabitants
};


GeoDataPlacemark::GeoDataPlacemark( GeoDataObject* parent )
    : GeoDataFeature( parent ),
      d( new GeoDataPlacemarkPrivate )
{
}

GeoDataPlacemark::GeoDataPlacemark( const GeoDataPlacemark& other )
: GeoDataFeature( other ),
  d( new GeoDataPlacemarkPrivate( *other.d ) )
{
}

GeoDataPlacemark::GeoDataPlacemark( const QString& name, GeoDataObject *parent )
    : GeoDataFeature( name, parent ),
      d( new GeoDataPlacemarkPrivate )
{
}

GeoDataPlacemark::~GeoDataPlacemark()
{
#ifdef DEBUG_GEODATA
    qDebug() << "delete Placemark";
#endif
    delete d;
}

void GeoDataPlacemark::operator=( const GeoDataPlacemark& other )
{
    *d = *other.d;
}

GeoDataGeometry* GeoDataPlacemark::geometry()
{
    if( d->m_geometry )
        return d->m_geometry;
    else
        return &( d->m_coordinate );
}

GeoDataCoordinates GeoDataPlacemark::coordinate() const
{
    return static_cast<GeoDataCoordinates>( d->m_coordinate );
}

void GeoDataPlacemark::coordinate( qreal& lon, qreal& lat, qreal& alt ) const
{
    d->m_coordinate.geoCoordinates( lon, lat );
    alt = d->m_coordinate.altitude();
}

void GeoDataPlacemark::setCoordinate( qreal lon, qreal lat, qreal alt )
{
    d->m_coordinate = GeoDataPoint( lon, lat, alt, this );
}

void GeoDataPlacemark::setCoordinate( const GeoDataPoint &point )
{
    d->m_coordinate = GeoDataPoint( point );
    d->m_coordinate.setParent( this );
}

void GeoDataPlacemark::setGeometry( const GeoDataPoint& point )
{
    delete d->m_geometry;
    d->m_geometry = new GeoDataPoint( point );
}

void GeoDataPlacemark::setGeometry( const GeoDataLineString& point )
{
    delete d->m_geometry;
    d->m_geometry = new GeoDataLineString( point );
}

void GeoDataPlacemark::setGeometry( const GeoDataLinearRing& point )
{
    delete d->m_geometry;
    d->m_geometry = new GeoDataLinearRing( point );
}

void GeoDataPlacemark::setGeometry( const GeoDataPolygon& point )
{
    delete d->m_geometry;
    d->m_geometry = new GeoDataPolygon( point );
}

void GeoDataPlacemark::setGeometry( const GeoDataMultiGeometry& point )
{
    delete d->m_geometry;
    d->m_geometry = new GeoDataMultiGeometry( point );
}

qreal GeoDataPlacemark::area() const
{
    return d->m_area;
}

void GeoDataPlacemark::setArea( qreal area )
{
    d->m_area = area;
}

qint64 GeoDataPlacemark::population() const
{
    return d->m_population;
}

void GeoDataPlacemark::setPopulation( qint64 population )
{
    d->m_population = population;
}

const QString GeoDataPlacemark::countryCode() const
{
    return d->m_countrycode;
}

void GeoDataPlacemark::setCountryCode( const QString &countrycode )
{
    d->m_countrycode = countrycode;
}

void GeoDataPlacemark::pack( QDataStream& stream ) const
{
    GeoDataFeature::pack( stream );

    stream << d->m_countrycode;
    stream << d->m_area;
    stream << d->m_population;

    stream << d->m_geometry->geometryId();
    d->m_geometry->pack( stream );
    d->m_coordinate.pack( stream );
}


void GeoDataPlacemark::unpack( QDataStream& stream )
{
    GeoDataFeature::unpack( stream );

    stream >> d->m_countrycode;
    stream >> d->m_area;
    stream >> d->m_population;

    int geometryId;
    stream >> geometryId;
    switch( geometryId ) {
        case InvalidGeometryId:
            break;
        case GeoDataPointId:
            {
            GeoDataPoint* point = new GeoDataPoint( this );
            point->unpack( stream );
            delete d->m_geometry;
            d->m_geometry = point;
            }
            break;
        case GeoDataLineStringId:
            {
            GeoDataLineString* lineString = new GeoDataLineString( this );
            lineString->unpack( stream );
            delete d->m_geometry;
            d->m_geometry = lineString;
            }
            break;
        case GeoDataLinearRingId:
            {
            GeoDataLinearRing* linearRing = new GeoDataLinearRing( this );
            linearRing->unpack( stream );
            delete d->m_geometry;
            d->m_geometry = linearRing;
            }
            break;
        case GeoDataPolygonId:
            {
            GeoDataPolygon* polygon = new GeoDataPolygon( this );
            polygon->unpack( stream );
            delete d->m_geometry;
            d->m_geometry = polygon;
            }
            break;
        case GeoDataMultiGeometryId:
            {
            GeoDataMultiGeometry* multiGeometry = new GeoDataMultiGeometry( this );
            multiGeometry->unpack( stream );
            delete d->m_geometry;
            d->m_geometry = multiGeometry;
            }
            break;
        case GeoDataModelId:
            break;
        default: break;
    };
    d->m_coordinate.unpack( stream );
}

}
