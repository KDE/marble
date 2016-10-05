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
#include "GeoDataExtendedData.h"

#include <QMap>
#include <QDateTime>

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
        m_when.reserve(m_coordinates.size());
        while ( m_when.size() < m_coordinates.size() ) {
            //fill coordinates without time information with null QDateTime
            m_when.append( QDateTime() );
        }
    }

    mutable GeoDataLineString m_lineString;
    mutable bool m_lineStringNeedsUpdate;

    bool m_interpolate;

    QVector<QDateTime> m_when;
    QVector<GeoDataCoordinates> m_coordinates;

    GeoDataExtendedData m_extendedData;
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
    Q_D(const GeoDataTrack);
    const GeoDataTrackPrivate * const otherD = other.d_func();

    return equals(other) &&
           d->m_when == otherD->m_when &&
           d->m_coordinates == otherD->m_coordinates &&
           d->m_extendedData == otherD->m_extendedData &&
           d->m_interpolate == otherD->m_interpolate;
}

bool GeoDataTrack::operator!=( const GeoDataTrack& other ) const
{
    return !this->operator==( other );
}

int GeoDataTrack::size() const
{
    Q_D(const GeoDataTrack);
    return d->m_coordinates.size();
}

bool GeoDataTrack::interpolate() const
{
    Q_D(const GeoDataTrack);
    return d->m_interpolate;
}

void GeoDataTrack::setInterpolate(bool on)
{
    detach();

    Q_D(GeoDataTrack);
    d->m_interpolate = on;
}

QDateTime GeoDataTrack::firstWhen() const
{
    Q_D(const GeoDataTrack);

    if (d->m_when.isEmpty()) {
        return QDateTime();
    }

    return d->m_when.first();
}

QDateTime GeoDataTrack::lastWhen() const
{
    Q_D(const GeoDataTrack);

    if (d->m_when.isEmpty()) {
        return QDateTime();
    }

    return d->m_when.last();
}

QVector<GeoDataCoordinates> GeoDataTrack::coordinatesList() const
{
    Q_D(const GeoDataTrack);
    return d->m_coordinates;
}

QVector<QDateTime> GeoDataTrack::whenList() const
{
    Q_D(const GeoDataTrack);
    return d->m_when;
}

GeoDataCoordinates GeoDataTrack::coordinatesAt( const QDateTime &when ) const
{
    Q_D(const GeoDataTrack);

    if (d->m_when.isEmpty()) {
        return GeoDataCoordinates();
    }

    if (d->m_when.contains(when)) {
        //exact match found
        const int index = d->m_when.indexOf(when);
        if (index < d->m_coordinates.size()) {
            return d->m_coordinates.at(index);
        }
    }

    if ( !interpolate() ) {
        return GeoDataCoordinates();
    }

    typedef QMap<QDateTime, GeoDataCoordinates> PointMap;
    PointMap pointMap;
    for (int i = 0; i < qMin(d->m_when.size(), d->m_coordinates.size()); ++i) {
        if (d->m_when.at(i).isValid()) {
            pointMap[d->m_when.at(i)] = d->m_coordinates.at(i);
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
    Q_D(const GeoDataTrack);
    return d->m_coordinates.at(index);
}

void GeoDataTrack::addPoint( const QDateTime &when, const GeoDataCoordinates &coord )
{
    detach();

    Q_D(GeoDataTrack);
    d->equalizeWhenSize();
    d->m_lineStringNeedsUpdate = true;
    int i=0;
    while (i < d->m_when.size()) {
        if (d->m_when.at(i) > when) {
            break;
        }
        ++i;
    }
    d->m_when.insert(i, when );
    d->m_coordinates.insert(i, coord );
}

void GeoDataTrack::appendCoordinates( const GeoDataCoordinates &coord )
{
    detach();

    Q_D(GeoDataTrack);
    d->equalizeWhenSize();
    d->m_lineStringNeedsUpdate = true;
    d->m_coordinates.append(coord);
}

void GeoDataTrack::appendAltitude( qreal altitude )
{
    detach();

    Q_D(GeoDataTrack);
    d->m_lineStringNeedsUpdate = true;
    Q_ASSERT(!d->m_coordinates.isEmpty());
    if (d->m_coordinates.isEmpty()) {
        return;
    }
    GeoDataCoordinates coordinates = d->m_coordinates.takeLast();
    coordinates.setAltitude( altitude );
    d->m_coordinates.append(coordinates);
}

void GeoDataTrack::appendWhen( const QDateTime &when )
{
    detach();

    Q_D(GeoDataTrack);
    d->m_when.append(when);
}

void GeoDataTrack::clear()
{
    detach();

    Q_D(GeoDataTrack);
    d->m_when.clear();
    d->m_coordinates.clear();
    d->m_lineStringNeedsUpdate = true;
}

void GeoDataTrack::removeBefore( const QDateTime &when )
{
    detach();

    Q_D(GeoDataTrack);
    Q_ASSERT( d->m_coordinates.size() == d->m_when.size());
    if (d->m_when.isEmpty()) {
        return;
    }
    d->equalizeWhenSize();

    while (!d->m_when.isEmpty() && d->m_when.first() < when) {
        d->m_when.takeFirst();
        d->m_coordinates.takeFirst();
    }
}

void GeoDataTrack::removeAfter( const QDateTime &when )
{
    detach();

    Q_D(GeoDataTrack);
    Q_ASSERT(d->m_coordinates.size() == d->m_when.size());
    if (d->m_when.isEmpty()) {
        return;
    }
    d->equalizeWhenSize();
    while (!d->m_when.isEmpty() && d->m_when.last() > when) {
        d->m_when.takeLast();
        d->m_coordinates.takeLast();
    }
}

const GeoDataLineString *GeoDataTrack::lineString() const
{
    Q_D(const GeoDataTrack);
    if (d->m_lineStringNeedsUpdate) {
        d->m_lineString = GeoDataLineString();
        d->m_lineString.append( coordinatesList() );
        d->m_lineStringNeedsUpdate = false;
    }
    return &d->m_lineString;
}

GeoDataExtendedData& GeoDataTrack::extendedData()
{
    detach();

    Q_D(GeoDataTrack);
    return d->m_extendedData;
}

const GeoDataExtendedData& GeoDataTrack::extendedData() const
{
    Q_D(const GeoDataTrack);
    return d->m_extendedData;
}

void GeoDataTrack::setExtendedData( const GeoDataExtendedData& extendedData )
{
    detach();

    Q_D(GeoDataTrack);
    d->m_extendedData = extendedData;
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
