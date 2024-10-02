// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
//

#include "GeoDataLatLonAltBox.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "MarbleDebug.h"

#include "GeoDataTypes.h"

#include <QDataStream>

namespace Marble
{

class GeoDataLatLonAltBoxPrivate
{
public:
    GeoDataLatLonAltBoxPrivate()
        : m_minAltitude(0)
        , m_maxAltitude(0)
        , m_altitudeMode(ClampToGround)
    {
    }

    qreal m_minAltitude;
    qreal m_maxAltitude;
    AltitudeMode m_altitudeMode;
};

bool operator==(GeoDataLatLonAltBox const &lhs, GeoDataLatLonAltBox const &rhs)
{
    return lhs.west() == rhs.west() && lhs.east() == rhs.east() && lhs.north() == rhs.north() && lhs.south() == rhs.south() && lhs.rotation() == rhs.rotation()
        && lhs.d->m_minAltitude == rhs.d->m_minAltitude && lhs.d->m_maxAltitude == rhs.d->m_maxAltitude && lhs.d->m_altitudeMode == rhs.d->m_altitudeMode;
}

GeoDataLatLonAltBox &GeoDataLatLonAltBox::operator=(const GeoDataLatLonAltBox &other)
{
    GeoDataLatLonBox::operator=(other);

    *d = *other.d;
    return *this;
}

GeoDataLatLonAltBox &GeoDataLatLonAltBox::operator=(const GeoDataCoordinates &other)
{
    setWest(other.longitude());
    setEast(other.longitude());
    setNorth(other.latitude());
    setSouth(other.latitude());
    setMinAltitude(other.altitude());
    setMaxAltitude(other.altitude());
    return *this;
}

GeoDataLatLonAltBox::GeoDataLatLonAltBox()
    : GeoDataLatLonBox()
    , d(new GeoDataLatLonAltBoxPrivate)
{
}

GeoDataLatLonAltBox::GeoDataLatLonAltBox(const GeoDataLatLonAltBox &other)
    : GeoDataLatLonBox(other)
    , d(new GeoDataLatLonAltBoxPrivate(*other.d))
{
}

GeoDataLatLonAltBox::GeoDataLatLonAltBox(const GeoDataLatLonBox &other, qreal minAltitude, qreal maxAltitude)
    : GeoDataLatLonBox(other)
    , d(new GeoDataLatLonAltBoxPrivate)
{
    setWest(other.west());
    setEast(other.east());
    setNorth(other.north());
    setSouth(other.south());
    setRotation(other.rotation());

    d->m_minAltitude = minAltitude;
    d->m_maxAltitude = maxAltitude;
}

GeoDataLatLonAltBox::GeoDataLatLonAltBox(const GeoDataCoordinates &coordinates)
    : GeoDataLatLonBox()
    , d(new GeoDataLatLonAltBoxPrivate)
{
    setWest(coordinates.longitude());
    setEast(coordinates.longitude());
    setNorth(coordinates.latitude());
    setSouth(coordinates.latitude());

    d->m_minAltitude = coordinates.altitude();
    d->m_maxAltitude = coordinates.altitude();
}

GeoDataLatLonAltBox::~GeoDataLatLonAltBox()
{
    delete d;
}

const char *GeoDataLatLonAltBox::nodeType() const
{
    return GeoDataTypes::GeoDataLatLonAltBoxType;
}

qreal GeoDataLatLonAltBox::minAltitude() const
{
    return d->m_minAltitude;
}

void GeoDataLatLonAltBox::setMinAltitude(const qreal minAltitude)
{
    d->m_minAltitude = minAltitude;
}

qreal GeoDataLatLonAltBox::maxAltitude() const
{
    return d->m_maxAltitude;
}

void GeoDataLatLonAltBox::setMaxAltitude(const qreal maxAltitude)
{
    d->m_maxAltitude = maxAltitude;
}

AltitudeMode GeoDataLatLonAltBox::altitudeMode() const
{
    return d->m_altitudeMode;
}

GeoDataCoordinates GeoDataLatLonAltBox::center() const
{
    if (isEmpty())
        return {};
    if (crossesDateLine())
        return {GeoDataCoordinates::normalizeLon(east() + 2 * M_PI - (east() + 2 * M_PI - west()) / 2),
                north() - (north() - south()) / 2,
                d->m_maxAltitude - (d->m_maxAltitude - d->m_minAltitude) / 2};
    else
        return {east() - (east() - west()) / 2, north() - (north() - south()) / 2, d->m_maxAltitude - (d->m_maxAltitude - d->m_minAltitude) / 2};
}

void GeoDataLatLonAltBox::setAltitudeMode(const AltitudeMode altitudeMode)
{
    d->m_altitudeMode = altitudeMode;
}

bool GeoDataLatLonAltBox::contains(const GeoDataCoordinates &point) const
{
    if (!GeoDataLatLonBox::contains(point))
        return false;

    if (point.altitude() < d->m_minAltitude || point.altitude() > d->m_maxAltitude) {
        return false;
    }

    return true;
}

bool GeoDataLatLonAltBox::contains(const GeoDataLatLonAltBox &other) const
{
    // check the contain criterion for the altitude first as this is trivial:

    // mDebug() << "this " << this->toString(GeoDataCoordinates::Degree);
    // mDebug() << "other" << other.toString(GeoDataCoordinates::Degree);

    if (d->m_maxAltitude >= other.maxAltitude() && d->m_minAltitude <= other.minAltitude()) {
        return GeoDataLatLonBox::contains(other);
    }

    return false;
}

bool GeoDataLatLonAltBox::intersects(const GeoDataLatLonAltBox &other) const
{
    // Case 1: maximum altitude of other box intersects:
    if ((d->m_maxAltitude >= other.maxAltitude() && d->m_minAltitude <= other.maxAltitude())
        // Case 2: maximum altitude of this box intersects:
        || (other.maxAltitude() >= d->m_maxAltitude && other.minAltitude() <= d->m_maxAltitude)
        // Case 3: minimum altitude of other box intersects:
        || (d->m_maxAltitude >= other.minAltitude() && d->m_minAltitude <= other.minAltitude())
        // Case 4: minimum altitude of this box intersects:
        || (other.maxAltitude() >= d->m_minAltitude && other.minAltitude() <= d->m_minAltitude)) {
        if (GeoDataLatLonBox::intersects(other))
            return true;
    }

    return false;
}

GeoDataLatLonAltBox GeoDataLatLonAltBox::fromLineString(const GeoDataLineString &lineString)
{
    // If the line string is empty return a boundingbox that contains everything
    if (lineString.size() == 0) {
        return {};
    }

    const qreal altitude = lineString.first().altitude();

    GeoDataLatLonAltBox temp(GeoDataLatLonBox::fromLineString(lineString), altitude, altitude);

    qreal maxAltitude = altitude;
    qreal minAltitude = altitude;

    // If there's only a single node stored then the boundingbox only contains that point
    if (lineString.size() == 1) {
        temp.setMinAltitude(minAltitude);
        temp.setMaxAltitude(maxAltitude);
        return temp;
    }

    QList<GeoDataCoordinates>::ConstIterator it(lineString.constBegin());
    QList<GeoDataCoordinates>::ConstIterator itEnd(lineString.constEnd());

    for (; it != itEnd; ++it) {
        // Get coordinates and normalize them to the desired range.
        const qreal altitude = (it)->altitude();

        // Determining the maximum and minimum altitude
        if (altitude > maxAltitude) {
            maxAltitude = altitude;
        } else if (altitude < minAltitude) {
            minAltitude = altitude;
        }
    }

    temp.setMinAltitude(minAltitude);
    temp.setMaxAltitude(maxAltitude);
    return temp;
}

bool GeoDataLatLonAltBox::isNull() const
{
    return GeoDataLatLonBox::isNull() && d->m_maxAltitude == d->m_minAltitude;
}

void GeoDataLatLonAltBox::clear()
{
    GeoDataLatLonBox::clear();
    d->m_minAltitude = 0;
    d->m_maxAltitude = 0;
    d->m_altitudeMode = ClampToGround;
}

void GeoDataLatLonAltBox::pack(QDataStream &stream) const
{
    GeoDataObject::pack(stream);

    stream << d->m_minAltitude << d->m_maxAltitude;
    stream << d->m_altitudeMode;
}

void GeoDataLatLonAltBox::unpack(QDataStream &stream)
{
    GeoDataObject::unpack(stream);

    stream >> d->m_minAltitude >> d->m_maxAltitude;
    int a;
    stream >> a;
    d->m_altitudeMode = static_cast<AltitudeMode>(a);
}

uint qHash(const GeoDataLatLonAltBox &box, uint seed)
{
    seed = ::qHash(box.east(), seed);
    seed = ::qHash(box.west(), seed);
    seed = ::qHash(box.south(), seed);
    seed = ::qHash(box.north(), seed);
    seed = ::qHash(box.maxAltitude(), seed);

    return ::qHash(box.minAltitude(), seed);
}

}
