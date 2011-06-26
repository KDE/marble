//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
//


// Own
#include "GeoDataPlacemark.h"

// Private
#include "GeoDataPlacemark_p.h"

#include "GeoDataMultiGeometry.h"
#include "GeoDataCoordinates.h"

// Qt
#include <QtCore/QDataStream>
#include "MarbleDebug.h"

namespace Marble
{
GeoDataPlacemark::GeoDataPlacemark()
    : GeoDataFeature( new GeoDataPlacemarkPrivate )
{
    p()->m_geometry->setParent( this );
}

GeoDataPlacemark::GeoDataPlacemark( const GeoDataPlacemark& other )
: GeoDataFeature( other )
{
    p()->m_geometry->setParent( this );
}

GeoDataPlacemark::GeoDataPlacemark( const QString& name )
    : GeoDataFeature( new GeoDataPlacemarkPrivate )
{
    d->m_name = name;
    p()->m_geometry->setParent( this );
}

GeoDataPlacemark::~GeoDataPlacemark()
{
}

bool GeoDataPlacemark::operator==( const GeoDataPlacemark& other ) const
{ 
    return p() == other.p();
}

GeoDataPlacemarkPrivate* GeoDataPlacemark::p() const
{
    return static_cast<GeoDataPlacemarkPrivate*>(d);
}

GeoDataGeometry* GeoDataPlacemark::geometry() const
{
    return p()->m_geometry;
}

GeoDataLookAt *GeoDataPlacemark::lookAt() const
{
    return p()->m_lookAt;
}

void GeoDataPlacemark::setLookAt( GeoDataLookAt *lookAt)
{
    p()->m_lookAt = lookAt;
}

GeoDataCoordinates GeoDataPlacemark::coordinate() const
{    
    if( p()->m_geometry ) {
        if ( p()->m_geometry->nodeType() != GeoDataTypes::GeoDataPointType ) {
            return p()->m_geometry->latLonAltBox().center();
        } else {
            return GeoDataCoordinates( *static_cast<GeoDataPoint*>( p()->m_geometry ) );
        }
    } else {
        return GeoDataCoordinates();
    }
}

void GeoDataPlacemark::coordinate( qreal& lon, qreal& lat, qreal& alt ) const
{
    if( dynamic_cast<GeoDataLineString*>( p()->m_geometry ) ) {
        const GeoDataCoordinates coord = GeoDataLatLonAltBox::fromLineString( *p()->m_geometry ).center();
        coord.geoCoordinates( lon, lat );
        alt = coord.altitude();
    } else if( dynamic_cast<GeoDataPolygon*>( p()->m_geometry ) ) {
        const GeoDataCoordinates coord = GeoDataLatLonAltBox::fromLineString( static_cast<GeoDataPolygon*>(p()->m_geometry)->outerBoundary() ).center();
        coord.geoCoordinates( lon, lat );
        alt = coord.altitude();
    } else {
        static_cast<GeoDataPoint*>(p()->m_geometry)->geoCoordinates( lon, lat );
        alt = static_cast<GeoDataPoint*>(p()->m_geometry)->altitude();
    }
}

void GeoDataPlacemark::setCoordinate( qreal lon, qreal lat, qreal alt, GeoDataPoint::Unit _unit)
{
    setGeometry( new GeoDataPoint(lon, lat, alt, _unit ) );
}

void GeoDataPlacemark::setCoordinate( const GeoDataPoint &point )
{
    setGeometry ( new GeoDataPoint( point ) );
}

void GeoDataPlacemark::setGeometry( GeoDataGeometry *entry )
{
    detach();
    delete p()->m_geometry;
    p()->m_geometry = entry;
    p()->m_geometry->setParent( this );
}

qreal GeoDataPlacemark::area() const
{
    return p()->m_area;
}

void GeoDataPlacemark::setArea( qreal area )
{
    detach();
    p()->m_area = area;
}

qint64 GeoDataPlacemark::population() const
{
    return p()->m_population;
}

void GeoDataPlacemark::setPopulation( qint64 population )
{
    detach();
    p()->m_population = population;
}

const QString GeoDataPlacemark::state() const
{
    return p()->m_state;
}

void GeoDataPlacemark::setState( const QString &state )
{
    detach();
    p()->m_state = state;
}

const QString GeoDataPlacemark::countryCode() const
{
    return p()->m_countrycode;
}

void GeoDataPlacemark::setCountryCode( const QString &countrycode )
{
    detach();
    p()->m_countrycode = countrycode;
}

void GeoDataPlacemark::pack( QDataStream& stream ) const
{
    GeoDataFeature::pack( stream );

    stream << p()->m_countrycode;
    stream << p()->m_area;
    stream << p()->m_population;
    if ( p()->m_geometry )
    {
        stream << p()->m_geometry->geometryId();
        p()->m_geometry->pack( stream );
    }
    else
    {
        stream << InvalidGeometryId;
    }
}

QXmlStreamWriter& GeoDataPlacemark::pack( QXmlStreamWriter& stream ) const
{
    stream.writeStartElement( "placemark" );

    stream.writeEndElement();
    return stream;
}

QXmlStreamWriter& GeoDataPlacemark::operator <<( QXmlStreamWriter& stream ) const
{
    pack( stream );
    return stream;
}

void GeoDataPlacemark::unpack( QDataStream& stream )
{
    detach();
    GeoDataFeature::unpack( stream );

    stream >> p()->m_countrycode;
    stream >> p()->m_area;
    stream >> p()->m_population;
    int geometryId;
    stream >> geometryId;
    switch( geometryId ) {
        case InvalidGeometryId:
            break;
        case GeoDataPointId:
            {
            GeoDataPoint* point = new GeoDataPoint;
            point->unpack( stream );
            delete p()->m_geometry;
            p()->m_geometry = point;
            }
            break;
        case GeoDataLineStringId:
            {
            GeoDataLineString* lineString = new GeoDataLineString;
            lineString->unpack( stream );
            delete p()->m_geometry;
            p()->m_geometry = lineString;
            }
            break;
        case GeoDataLinearRingId:
            {
            GeoDataLinearRing* linearRing = new GeoDataLinearRing;
            linearRing->unpack( stream );
            delete p()->m_geometry;
            p()->m_geometry = linearRing;
            }
            break;
        case GeoDataPolygonId:
            {
            GeoDataPolygon* polygon = new GeoDataPolygon;
            polygon->unpack( stream );
            delete p()->m_geometry;
            p()->m_geometry = polygon;
            }
            break;
        case GeoDataMultiGeometryId:
            {
            GeoDataMultiGeometry* multiGeometry = new GeoDataMultiGeometry;
            multiGeometry->unpack( stream );
            delete p()->m_geometry;
            p()->m_geometry = multiGeometry;
            }
            break;
        case GeoDataModelId:
            break;
        default: break;
    };
}

}
