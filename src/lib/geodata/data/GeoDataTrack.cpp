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

#include "GeoDataLatLonAltBox.h"
#include "GeoDataTypes.h"
#include "MarbleDebug.h"
#include "Quaternion.h"

#include "GeoDataLineString.h"

#include <QtCore/QMap>
#include <QtCore/QLinkedList>
#include "GeoDataExtendedData.h"

namespace Marble {

class GeoDataTrackPrivate
{
public:
    GeoDataTrackPrivate()
        : m_lineString( new GeoDataLineString() ),
          m_lineStringNeedsUpdate( false ),
          m_interpolate( false )
    {
    }

    void equalizeWhenSize()
    {
        while ( m_when.size() < m_coordinates.size() ) {
            //fill coordinates without time information with null QDateTime
            m_when.append( QDateTime() );
        }
    }

    GeoDataLineString *m_lineString;
    bool m_lineStringNeedsUpdate;

    QList<QDateTime> m_when;
    QList<GeoDataCoordinates> m_coordinates;

    GeoDataExtendedData m_extendedData;

    bool m_interpolate;
};

GeoDataTrack::GeoDataTrack()
    : d( new GeoDataTrackPrivate() )
{

}

GeoDataTrack::GeoDataTrack( const GeoDataGeometry &other )
    : GeoDataGeometry( other )
{

}

int GeoDataTrack::size() const
{
    return d->m_coordinates.size();
}

bool GeoDataTrack::interpolate() const
{
    return d->m_interpolate;
}

void GeoDataTrack::setInterpolate(bool on)
{
    d->m_interpolate = on;
}

QDateTime GeoDataTrack::firstWhen() const
{
    if ( d->m_when.isEmpty() ) {
        return QDateTime();
    }

    return d->m_when.first();
}

QDateTime GeoDataTrack::lastWhen() const
{
    if ( d->m_when.isEmpty() ) {
        return QDateTime();
    }

    return d->m_when.last();
}

QList<GeoDataCoordinates> GeoDataTrack::coordinatesList() const
{
    return d->m_coordinates;
}

QList<QDateTime> GeoDataTrack::whenList() const
{
    return d->m_when;
}

GeoDataCoordinates GeoDataTrack::coordinatesAt( const QDateTime &when ) const
{
    if ( d->m_when.isEmpty() ) {
        return GeoDataCoordinates();
    }

    if ( d->m_when.contains( when ) ) {
        //exact match found
        int index = d->m_when.indexOf( when );
        if ( index < d->m_coordinates.size() ) {
            return d->m_coordinates.at( index );
        }
    }

    if ( !interpolate() ) {
        return GeoDataCoordinates();
    }

    typedef QMap<QDateTime, GeoDataCoordinates> PointMap;
    PointMap pointMap;
    for ( int i = 0; i < qMin( d->m_when.size(), d->m_coordinates.size() ); ++i) {
        if ( d->m_when.at( i ).isValid() ) {
            pointMap[ d->m_when.at( i ) ] = d->m_coordinates.at( i );
        }
    }

    QMap<QDateTime, GeoDataCoordinates>::const_iterator nextEntry = const_cast<const PointMap&>(pointMap).upperBound( when );

    // No tracked point happened before "when"
    if ( nextEntry == pointMap.constBegin() ) {
        mDebug() << "No tracked point before " << when;
        return GeoDataCoordinates();
    }

    QMap<QDateTime, GeoDataCoordinates>::const_iterator previousEntry = nextEntry - 1;
    GeoDataCoordinates previousCoord = previousEntry.value();

    QDateTime previousWhen = previousEntry.key();
    QDateTime nextWhen = nextEntry.key();
    GeoDataCoordinates nextCoord = nextEntry.value();

#if QT_VERSION < 0x040700	
    int interval = 1000 * previousWhen.secsTo( nextWhen );
    int position = 1000 * previousWhen.secsTo( when );
#else	
    int interval = previousWhen.msecsTo( nextWhen );
    int position = previousWhen.msecsTo( when );
#endif	
    qreal t = (qreal)position / (qreal)interval;

    Quaternion interpolated;
    interpolated.slerp( previousCoord.quaternion(), nextCoord.quaternion(), t );
    qreal lon, lat;
    interpolated.getSpherical( lon, lat );

    qreal alt = previousCoord.altitude() + ( nextCoord.altitude() - previousCoord.altitude() ) * t;

    return GeoDataCoordinates( lon, lat, alt );
}

GeoDataCoordinates GeoDataTrack::coordinatesAt( int index ) const
{
    return d->m_coordinates.at( index );
}

void GeoDataTrack::addPoint( const QDateTime &when, const GeoDataCoordinates &coord )
{
    d->equalizeWhenSize();
    d->m_lineStringNeedsUpdate = true;
    int i=0;
    while ( i < d->m_when.size() ) {
        if ( d->m_when.at( i ) > when ) {
            break;
        }
        ++i;
    }
    d->m_when.insert(i, when );
    d->m_coordinates.insert(i, coord );
}

void GeoDataTrack::appendCoordinates( const GeoDataCoordinates &coord )
{
    d->equalizeWhenSize();
    d->m_lineStringNeedsUpdate = true;
    d->m_coordinates.append( coord );
}

void GeoDataTrack::appendAltitude( qreal altitude )
{
    d->m_lineStringNeedsUpdate = true;
    Q_ASSERT( !d->m_coordinates.isEmpty() );
    if ( d->m_coordinates.isEmpty() ) return;
    GeoDataCoordinates coordinates = d->m_coordinates.takeLast();
    coordinates.setAltitude( altitude );
    d->m_coordinates.append( coordinates );
}

void GeoDataTrack::appendWhen( const QDateTime &when )
{
    d->m_when.append( when );
}

void GeoDataTrack::clear()
{
    d->m_when.clear();
    d->m_coordinates.clear();
    d->m_lineStringNeedsUpdate = true;
}

void GeoDataTrack::removeBefore( const QDateTime &when )
{
    Q_ASSERT( d->m_coordinates.size() == d->m_when.size() );
    if ( d->m_when.isEmpty() ) {
        return;
    }
    d->equalizeWhenSize();

    while ( !d->m_when.isEmpty() && d->m_when.first() < when ) {
        d->m_when.takeFirst();
        d->m_coordinates.takeFirst();
    }
}

void GeoDataTrack::removeAfter( const QDateTime &when )
{
    Q_ASSERT( d->m_coordinates.size() == d->m_when.size() );
    if ( d->m_when.isEmpty() ) {
        return;
    }
    d->equalizeWhenSize();
    while ( !d->m_when.isEmpty() && d->m_when.last() > when ) {
        d->m_when.takeLast();
        d->m_coordinates.takeLast();

    }
}

const GeoDataLineString *GeoDataTrack::lineString() const
{
    if ( d->m_lineStringNeedsUpdate ) {
        delete d->m_lineString;
        d->m_lineString = new GeoDataLineString();
        foreach ( const GeoDataCoordinates &coordinates, coordinatesList() ) {
            d->m_lineString->append( coordinates );
        }
        d->m_lineStringNeedsUpdate = false;
    }
    return d->m_lineString;
}

GeoDataExtendedData& GeoDataTrack::extendedData() const
{
    return d->m_extendedData;
}

void GeoDataTrack::setExtendedData( const GeoDataExtendedData& extendedData )
{
    d->m_extendedData = extendedData;
}

const char* GeoDataTrack::nodeType() const
{
    return GeoDataTypes::GeoDataTrackType;
}

EnumGeometryId GeoDataTrack::geometryId() const
{
    return GeoDataTrackId;
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

}
