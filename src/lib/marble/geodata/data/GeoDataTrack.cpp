//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "GeoDataTrack.h"
#include "GeoDataGeometry_p.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataTypes.h"
#include "MarbleDebug.h"
#include "Quaternion.h"

#include "GeoDataLineString.h"

#include <QMap>
#include <QLinkedList>
#include "GeoDataExtendedData.h"

namespace Marble {

class GeoDataTrackPrivate : public GeoDataGeometryPrivate
{
public:
    GeoDataTrackPrivate()
        : m_lineStringNeedsUpdate( false ),
          m_interpolate( false )
    {
    }

    const char *nodeType() const { return GeoDataTypes::GeoDataTrackType; }

    GeoDataGeometryPrivate *copy() { return new GeoDataTrackPrivate( *this ); }

    EnumGeometryId geometryId() const { return GeoDataTrackId; }

    void equalizeWhenSize()
    {
        while ( m_when.size() < m_coordinates.size() ) {
            //fill coordinates without time information with null QDateTime
            m_when.append( QDateTime() );
        }
    }

    GeoDataLineString m_lineString;
    bool m_lineStringNeedsUpdate;

    QList<QDateTime> m_when;
    QList<GeoDataCoordinates> m_coordinates;

    GeoDataExtendedData m_extendedData;

    bool m_interpolate;
};

GeoDataTrack::GeoDataTrack() :
    GeoDataGeometry( new GeoDataTrackPrivate() )
{

}

GeoDataTrack::GeoDataTrack( const GeoDataTrack &other )
    : GeoDataGeometry( other )
{

}

GeoDataTrack &GeoDataTrack::operator=( const GeoDataTrack &other )
{
    GeoDataGeometry::operator=( other );

    return *this;
}


bool GeoDataTrack::operator==( const GeoDataTrack& other ) const
{
    return equals(other) &&
           p()->m_when == other.p()->m_when &&
           p()->m_coordinates == other.p()->m_coordinates &&
           p()->m_extendedData == other.p()->m_extendedData &&
           p()->m_interpolate == other.p()->m_interpolate;
}

bool GeoDataTrack::operator!=( const GeoDataTrack& other ) const
{
    return !this->operator==( other );
}

int GeoDataTrack::size() const
{
    return p()->m_coordinates.size();
}

bool GeoDataTrack::interpolate() const
{
    return p()->m_interpolate;
}

void GeoDataTrack::setInterpolate(bool on)
{
    detach();

    p()->m_interpolate = on;
}

QDateTime GeoDataTrack::firstWhen() const
{
    if ( p()->m_when.isEmpty() ) {
        return QDateTime();
    }

    return p()->m_when.first();
}

QDateTime GeoDataTrack::lastWhen() const
{
    if ( p()->m_when.isEmpty() ) {
        return QDateTime();
    }

    return p()->m_when.last();
}

QList<GeoDataCoordinates> GeoDataTrack::coordinatesList() const
{
    return p()->m_coordinates;
}

QList<QDateTime> GeoDataTrack::whenList() const
{
    return p()->m_when;
}

GeoDataCoordinates GeoDataTrack::coordinatesAt( const QDateTime &when ) const
{
    if ( p()->m_when.isEmpty() ) {
        return GeoDataCoordinates();
    }

    if ( p()->m_when.contains( when ) ) {
        //exact match found
        int index = p()->m_when.indexOf( when );
        if ( index < p()->m_coordinates.size() ) {
            return p()->m_coordinates.at( index );
        }
    }

    if ( !interpolate() ) {
        return GeoDataCoordinates();
    }

    typedef QMap<QDateTime, GeoDataCoordinates> PointMap;
    PointMap pointMap;
    for ( int i = 0; i < qMin( p()->m_when.size(), p()->m_coordinates.size() ); ++i) {
        if ( p()->m_when.at( i ).isValid() ) {
            pointMap[ p()->m_when.at( i ) ] = p()->m_coordinates.at( i );
        }
    }

    QMap<QDateTime, GeoDataCoordinates>::const_iterator nextEntry = const_cast<const PointMap&>(pointMap).upperBound( when );

    // No tracked point happened before "when"
    if ( nextEntry == pointMap.constBegin() ) {
        mDebug() << "No tracked point before " << when;
        return GeoDataCoordinates();
    }

    if ( nextEntry == pointMap.constEnd() ) {
        mDebug() << "No track point after" << when;
        return GeoDataCoordinates();
    }

    QMap<QDateTime, GeoDataCoordinates>::const_iterator previousEntry = nextEntry - 1;
    GeoDataCoordinates previousCoord = previousEntry.value();

    QDateTime previousWhen = previousEntry.key();
    QDateTime nextWhen = nextEntry.key();
    GeoDataCoordinates nextCoord = nextEntry.value();

    int interval = previousWhen.msecsTo( nextWhen );
    int position = previousWhen.msecsTo( when );
    qreal t = (qreal)position / (qreal)interval;

    const Quaternion interpolated = Quaternion::slerp( previousCoord.quaternion(), nextCoord.quaternion(), t );
    qreal lon, lat;
    interpolated.getSpherical( lon, lat );

    qreal alt = previousCoord.altitude() + ( nextCoord.altitude() - previousCoord.altitude() ) * t;

    return GeoDataCoordinates( lon, lat, alt );
}

GeoDataCoordinates GeoDataTrack::coordinatesAt( int index ) const
{
    return p()->m_coordinates.at( index );
}

void GeoDataTrack::addPoint( const QDateTime &when, const GeoDataCoordinates &coord )
{
    detach();

    p()->equalizeWhenSize();
    p()->m_lineStringNeedsUpdate = true;
    int i=0;
    while ( i < p()->m_when.size() ) {
        if ( p()->m_when.at( i ) > when ) {
            break;
        }
        ++i;
    }
    p()->m_when.insert(i, when );
    p()->m_coordinates.insert(i, coord );
}

void GeoDataTrack::appendCoordinates( const GeoDataCoordinates &coord )
{
    detach();

    p()->equalizeWhenSize();
    p()->m_lineStringNeedsUpdate = true;
    p()->m_coordinates.append( coord );
}

void GeoDataTrack::appendAltitude( qreal altitude )
{
    detach();

    p()->m_lineStringNeedsUpdate = true;
    Q_ASSERT( !p()->m_coordinates.isEmpty() );
    if ( p()->m_coordinates.isEmpty() ) return;
    GeoDataCoordinates coordinates = p()->m_coordinates.takeLast();
    coordinates.setAltitude( altitude );
    p()->m_coordinates.append( coordinates );
}

void GeoDataTrack::appendWhen( const QDateTime &when )
{
    detach();

    p()->m_when.append( when );
}

void GeoDataTrack::clear()
{
    detach();

    p()->m_when.clear();
    p()->m_coordinates.clear();
    p()->m_lineStringNeedsUpdate = true;
}

void GeoDataTrack::removeBefore( const QDateTime &when )
{
    detach();

    Q_ASSERT( p()->m_coordinates.size() == p()->m_when.size() );
    if ( p()->m_when.isEmpty() ) {
        return;
    }
    p()->equalizeWhenSize();

    while ( !p()->m_when.isEmpty() && p()->m_when.first() < when ) {
        p()->m_when.takeFirst();
        p()->m_coordinates.takeFirst();
    }
}

void GeoDataTrack::removeAfter( const QDateTime &when )
{
    detach();

    Q_ASSERT( p()->m_coordinates.size() == p()->m_when.size() );
    if ( p()->m_when.isEmpty() ) {
        return;
    }
    p()->equalizeWhenSize();
    while ( !p()->m_when.isEmpty() && p()->m_when.last() > when ) {
        p()->m_when.takeLast();
        p()->m_coordinates.takeLast();

    }
}

const GeoDataLineString *GeoDataTrack::lineString() const
{
    if ( p()->m_lineStringNeedsUpdate ) {
        p()->m_lineString = GeoDataLineString();
        foreach ( const GeoDataCoordinates &coordinates, coordinatesList() ) {
            p()->m_lineString.append( coordinates );
        }
        p()->m_lineStringNeedsUpdate = false;
    }
    return &p()->m_lineString;
}

GeoDataExtendedData& GeoDataTrack::extendedData() const
{
    return p()->m_extendedData;
}

void GeoDataTrack::setExtendedData( const GeoDataExtendedData& extendedData )
{
    detach();

    p()->m_extendedData = extendedData;
}

const GeoDataLatLonAltBox& GeoDataTrack::latLonAltBox() const
{
    return lineString()->latLonAltBox();
}

//TODO
void GeoDataTrack::pack( QDataStream& stream ) const
{
    GeoDataGeometry::pack( stream );
}
//TODO
void GeoDataTrack::unpack( QDataStream& stream )
{
    GeoDataGeometry::unpack( stream );
}

GeoDataTrackPrivate *GeoDataTrack::p() const
{
    return static_cast<GeoDataTrackPrivate *>( d );
}

}
