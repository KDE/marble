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
#include <QDataStream>
#include "MarbleDebug.h"
#include "GeoDataTrack.h"
#include "GeoDataModel.h"

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
    // FIXME: temporary (until detach() is called) violates following invariant
    // which could lead to crashes
//    Q_ASSERT( this == p()->m_geometry->parent() );

    // FIXME: fails as well when "other" is a copy where detach wasn't called
//    Q_ASSERT( other.p()->m_geometry == 0 || &other == other.p()->m_geometry->parent() );
}

GeoDataPlacemark::GeoDataPlacemark( const QString& name )
    : GeoDataFeature( new GeoDataPlacemarkPrivate )
{
    d->m_name = name;
    p()->m_geometry->setParent( this );
}

GeoDataPlacemark::~GeoDataPlacemark()
{
    // nothing to do
}

GeoDataPlacemark &GeoDataPlacemark::operator=( const GeoDataPlacemark &other )
{
    GeoDataFeature::operator=( other );

    return *this;
}

bool GeoDataPlacemark::operator==( const GeoDataPlacemark& other ) const
{ 
    if ( !equals(other) ||
         p()->m_countrycode != other.p()->m_countrycode ||
         p()->m_area != other.p()->m_area ||
         p()->m_population != other.p()->m_population ||
         p()->m_state != other.p()->m_state ) {
        return false;
    }

    if ( !p()->m_geometry && !other.p()->m_geometry ) {
        return true;
    } else if ( (!p()->m_geometry && other.p()->m_geometry) ||
                (p()->m_geometry && !other.p()->m_geometry) ) {
        return false;
    }

    if ( p()->m_geometry->nodeType() != other.p()->m_geometry->nodeType() ) {
        return false;
    }

    if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        GeoDataPolygon *thisPoly = dynamic_cast<GeoDataPolygon*>( p()->m_geometry );
        GeoDataPolygon *otherPoly = dynamic_cast<GeoDataPolygon*>( other.p()->m_geometry );
        Q_ASSERT( thisPoly && otherPoly );

        if ( *thisPoly != *otherPoly ) {
            return false;
        }
    } else if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
        GeoDataLineString *thisLine = dynamic_cast<GeoDataLineString*>( p()->m_geometry );
        GeoDataLineString *otherLine = dynamic_cast<GeoDataLineString*>( other.p()->m_geometry );
        Q_ASSERT( thisLine && otherLine );

        if ( *thisLine != *otherLine ) {
            return false;
        }
    } else if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataModelType ) {
        GeoDataModel *thisModel = dynamic_cast<GeoDataModel*>( p()->m_geometry );
        GeoDataModel *otherModel = dynamic_cast<GeoDataModel*>( other.p()->m_geometry );
        Q_ASSERT( thisModel && otherModel );

        if ( *thisModel != *otherModel ) {
            return false;
        }
    /*} else if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) {
        GeoDataMultiGeometry *thisMG = dynamic_cast<GeoDataMultiGeometry*>( p()->m_geometry );
        GeoDataMultiGeometry *otherMG = dynamic_cast<GeoDataMultiGeometry*>( other.p()->m_geometry );
        Q_ASSERT( thisMG && otherMG );

        if ( *thisMG != *otherMG ) {
            return false;
        } */ // Does not have equality operators. I guess they need to be implemented soon.
    } else if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataTrackType ) {
        GeoDataTrack *thisTrack = dynamic_cast<GeoDataTrack*>( p()->m_geometry );
        GeoDataTrack *otherTrack = dynamic_cast<GeoDataTrack*>( other.p()->m_geometry );
        Q_ASSERT( thisTrack && otherTrack );

        if ( *thisTrack != *otherTrack ) {
            return false;
        }
    } else if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataMultiTrackType ) {
        GeoDataMultiTrack *thisMT = dynamic_cast<GeoDataMultiTrack*>( p()->m_geometry );
        GeoDataMultiTrack *otherMT = dynamic_cast<GeoDataMultiTrack*>( other.p()->m_geometry );
        Q_ASSERT( thisMT && otherMT );

        if ( *thisMT != *otherMT ) {
            return false;
        }
    } else if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataPointType ) {
        GeoDataPoint *thisPoint = dynamic_cast<GeoDataPoint*>( p()->m_geometry );
        GeoDataPoint *otherPoint = dynamic_cast<GeoDataPoint*>( other.p()->m_geometry );
        Q_ASSERT( thisPoint && otherPoint );

        if ( *thisPoint != *otherPoint ) {
            return false;
        }
    }

    return true;
}

bool GeoDataPlacemark::operator!=( const GeoDataPlacemark& other ) const
{
    return !this->operator==( other );
}

GeoDataPlacemarkPrivate* GeoDataPlacemark::p()
{
    return static_cast<GeoDataPlacemarkPrivate*>(d);
}

const GeoDataPlacemarkPrivate* GeoDataPlacemark::p() const
{
    return static_cast<GeoDataPlacemarkPrivate*>(d);
}

GeoDataGeometry* GeoDataPlacemark::geometry()
{
    detach();
    p()->m_geometry->setParent( this );
    return p()->m_geometry;
}

const GeoDataGeometry* GeoDataPlacemark::geometry() const
{
    return p()->m_geometry;
}

const GeoDataLookAt *GeoDataPlacemark::lookAt() const
{
    return dynamic_cast<const GeoDataLookAt*>( abstractView() );
}

GeoDataLookAt *GeoDataPlacemark::lookAt()
{
    return dynamic_cast<GeoDataLookAt*>( abstractView() );
}

GeoDataCoordinates GeoDataPlacemark::coordinate( const QDateTime &dateTime, bool *iconAtCoordinates ) const
{
    bool hasIcon = false;
    GeoDataCoordinates coord;
 
    if( p()->m_geometry ) {
        // Beware: comparison between pointers, not strings.
        if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataPointType ) {
            hasIcon = true;
            coord = static_cast<const GeoDataPoint *>( p()->m_geometry )->coordinates();
        } else if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataMultiGeometryType ) {
            const GeoDataMultiGeometry *multiGeometry = static_cast<const GeoDataMultiGeometry *>( p()->m_geometry );

            QVector<GeoDataGeometry*>::ConstIterator it = multiGeometry->constBegin();
            QVector<GeoDataGeometry*>::ConstIterator end = multiGeometry->constEnd();
            for ( ; it != end; ++it ) {
                if ( (*it)->nodeType() == GeoDataTypes::GeoDataPointType ) {
                    hasIcon = true;
                    break;
                }
            }

            coord = p()->m_geometry->latLonAltBox().center();
        } else if ( p()->m_geometry->nodeType() == GeoDataTypes::GeoDataTrackType ) {
            const GeoDataTrack *track = static_cast<const GeoDataTrack *>( p()->m_geometry );
            hasIcon = track->size() != 0 && track->firstWhen() <= dateTime;
            coord = track->coordinatesAt( dateTime );
        } else {
            coord = p()->m_geometry->latLonAltBox().center();
        }
    }

    if ( iconAtCoordinates != 0 ) {
        *iconAtCoordinates = hasIcon;
    }
    return coord;
}

void GeoDataPlacemark::coordinate( qreal& lon, qreal& lat, qreal& alt ) const
{
    coordinate().geoCoordinates( lon, lat, alt );
}

void GeoDataPlacemark::setCoordinate( qreal lon, qreal lat, qreal alt, GeoDataPoint::Unit _unit)
{
    setGeometry( new GeoDataPoint(lon, lat, alt, _unit ) );
}

void GeoDataPlacemark::setCoordinate( const GeoDataCoordinates &point )
{
    setGeometry ( new GeoDataPoint( point ) );
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
    p()->m_geometry->setParent( this );
    p()->m_area = area;
}

qint64 GeoDataPlacemark::population() const
{
    return p()->m_population;
}

void GeoDataPlacemark::setPopulation( qint64 population )
{
    detach();
    p()->m_geometry->setParent( this );
    p()->m_population = population;
}

const QString GeoDataPlacemark::state() const
{
    return p()->m_state;
}

void GeoDataPlacemark::setState( const QString &state )
{
    detach();
    p()->m_geometry->setParent( this );
    p()->m_state = state;
}

const QString GeoDataPlacemark::countryCode() const
{
    return p()->m_countrycode;
}

void GeoDataPlacemark::setCountryCode( const QString &countrycode )
{
    detach();
    p()->m_geometry->setParent( this );
    p()->m_countrycode = countrycode;
}

bool GeoDataPlacemark::isBalloonVisible() const
{
    return p()->m_isBalloonVisible;
}

void GeoDataPlacemark::setBalloonVisible( bool visible )
{
    detach();
    p()->m_geometry->setParent( this );
    p()->m_isBalloonVisible = visible;
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
    p()->m_geometry->setParent( this );
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
