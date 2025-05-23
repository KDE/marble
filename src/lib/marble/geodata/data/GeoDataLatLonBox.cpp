// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2008-2009 Torsten Rahn <rahn@kde.org>
//

#include "GeoDataLatLonBox.h"

#include "GeoDataLineString.h"
#include "MarbleDebug.h"

#include "GeoDataTypes.h"

#include <QDataStream>

namespace Marble
{

const GeoDataLatLonBox GeoDataLatLonBox::empty = GeoDataLatLonBox();

class GeoDataLatLonBoxPrivate
{
public:
    GeoDataLatLonBoxPrivate()
        : m_north(0.0)
        , m_south(0.0)
        , m_east(0.0)
        , m_west(0.0)
        , m_rotation(0.0)
    {
    }

    qreal m_north;
    qreal m_south;
    qreal m_east;
    qreal m_west;
    qreal m_rotation; // NOT implemented yet!
};

bool operator==(GeoDataLatLonBox const &lhs, GeoDataLatLonBox const &rhs)
{
    return lhs.d->m_west == rhs.d->m_west && lhs.d->m_east == rhs.d->m_east && lhs.d->m_north == rhs.d->m_north && lhs.d->m_south == rhs.d->m_south
        && lhs.d->m_rotation == rhs.d->m_rotation;
}

bool operator!=(GeoDataLatLonBox const &lhs, GeoDataLatLonBox const &rhs)
{
    return !(lhs == rhs);
}

GeoDataLatLonBox::GeoDataLatLonBox()
    : GeoDataObject()
    , d(new GeoDataLatLonBoxPrivate)
{
}

GeoDataLatLonBox::GeoDataLatLonBox(qreal north, qreal south, qreal east, qreal west, GeoDataCoordinates::Unit unit)
    : GeoDataObject()
    , d(new GeoDataLatLonBoxPrivate)
{
    setBoundaries(north, south, east, west, unit);
}

GeoDataLatLonBox::GeoDataLatLonBox(const GeoDataLatLonBox &other)
    : GeoDataObject(other)
    , d(new GeoDataLatLonBoxPrivate(*other.d))
{
}

GeoDataLatLonBox::~GeoDataLatLonBox()
{
    delete d;
}

const char *GeoDataLatLonBox::nodeType() const
{
    return GeoDataTypes::GeoDataLatLonBoxType;
}

qreal GeoDataLatLonBox::north(GeoDataCoordinates::Unit unit) const
{
    if (unit == GeoDataCoordinates::Degree) {
        return d->m_north * RAD2DEG;
    }
    return d->m_north;
}

void GeoDataLatLonBox::setNorth(const qreal north, GeoDataCoordinates::Unit unit)
{
    switch (unit) {
    default:
    case GeoDataCoordinates::Radian:
        d->m_north = GeoDataCoordinates::normalizeLat(north);
        break;
    case GeoDataCoordinates::Degree:
        d->m_north = GeoDataCoordinates::normalizeLat(north * DEG2RAD);
        break;
    }
}

qreal GeoDataLatLonBox::south(GeoDataCoordinates::Unit unit) const
{
    if (unit == GeoDataCoordinates::Degree) {
        return d->m_south * RAD2DEG;
    }
    return d->m_south;
}

void GeoDataLatLonBox::setSouth(const qreal south, GeoDataCoordinates::Unit unit)
{
    switch (unit) {
    default:
    case GeoDataCoordinates::Radian:
        d->m_south = GeoDataCoordinates::normalizeLat(south);
        break;
    case GeoDataCoordinates::Degree:
        d->m_south = GeoDataCoordinates::normalizeLat(south * DEG2RAD);
        break;
    }
}

qreal GeoDataLatLonBox::east(GeoDataCoordinates::Unit unit) const
{
    if (unit == GeoDataCoordinates::Degree) {
        return d->m_east * RAD2DEG;
    }
    return d->m_east;
}

void GeoDataLatLonBox::setEast(const qreal east, GeoDataCoordinates::Unit unit)
{
    switch (unit) {
    default:
    case GeoDataCoordinates::Radian:
        d->m_east = GeoDataCoordinates::normalizeLon(east);
        break;
    case GeoDataCoordinates::Degree:
        d->m_east = GeoDataCoordinates::normalizeLon(east * DEG2RAD);
        break;
    }
}

qreal GeoDataLatLonBox::west(GeoDataCoordinates::Unit unit) const
{
    if (unit == GeoDataCoordinates::Degree) {
        return d->m_west * RAD2DEG;
    }
    return d->m_west;
}

void GeoDataLatLonBox::setWest(const qreal west, GeoDataCoordinates::Unit unit)
{
    switch (unit) {
    default:
    case GeoDataCoordinates::Radian:
        d->m_west = GeoDataCoordinates::normalizeLon(west);
        break;
    case GeoDataCoordinates::Degree:
        d->m_west = GeoDataCoordinates::normalizeLon(west * DEG2RAD);
        break;
    }
}

void GeoDataLatLonBox::setRotation(const qreal rotation, GeoDataCoordinates::Unit unit)
{
    switch (unit) {
    default:
    case GeoDataCoordinates::Radian:
        d->m_rotation = rotation;
        break;
    case GeoDataCoordinates::Degree:
        d->m_rotation = rotation * DEG2RAD;
        break;
    }
}

qreal GeoDataLatLonBox::rotation(GeoDataCoordinates::Unit unit) const
{
    if (unit == GeoDataCoordinates::Degree) {
        return d->m_rotation * RAD2DEG;
    }
    return d->m_rotation;
}

void GeoDataLatLonBox::boundaries(qreal &north, qreal &south, qreal &east, qreal &west, GeoDataCoordinates::Unit unit) const
{
    switch (unit) {
    default:
    case GeoDataCoordinates::Radian:
        north = d->m_north;
        south = d->m_south;
        east = d->m_east;
        west = d->m_west;
        break;
    case GeoDataCoordinates::Degree:
        north = d->m_north * RAD2DEG;
        south = d->m_south * RAD2DEG;
        east = d->m_east * RAD2DEG;
        west = d->m_west * RAD2DEG;
        break;
    }
}

void GeoDataLatLonBox::setBoundaries(qreal north, qreal south, qreal east, qreal west, GeoDataCoordinates::Unit unit)
{
    switch (unit) {
    default:
    case GeoDataCoordinates::Radian:
        d->m_north = GeoDataCoordinates::normalizeLat(north);
        d->m_south = GeoDataCoordinates::normalizeLat(south);
        d->m_east = GeoDataCoordinates::normalizeLon(east);
        d->m_west = GeoDataCoordinates::normalizeLon(west);
        break;
    case GeoDataCoordinates::Degree:
        d->m_north = GeoDataCoordinates::normalizeLat(north * DEG2RAD);
        d->m_south = GeoDataCoordinates::normalizeLat(south * DEG2RAD);
        d->m_east = GeoDataCoordinates::normalizeLon(east * DEG2RAD);
        d->m_west = GeoDataCoordinates::normalizeLon(west * DEG2RAD);
        break;
    }
}

void GeoDataLatLonBox::scale(qreal verticalFactor, qreal horizontalFactor) const
{
    GeoDataCoordinates const middle = center();
    qreal const deltaY = 0.5 * height() * verticalFactor;
    qreal const deltaX = 0.5 * width() * horizontalFactor;
    d->m_north = qMin((middle.latitude() + deltaY), static_cast<qreal>(M_PI / 2));
    d->m_south = qMax((middle.latitude() - deltaY), static_cast<qreal>(-M_PI / 2));
    if (deltaX > 180 * DEG2RAD) {
        d->m_east = M_PI;
        d->m_west = -M_PI;
    } else {
        d->m_east = GeoDataCoordinates::normalizeLon(middle.longitude() + deltaX);
        d->m_west = GeoDataCoordinates::normalizeLon(middle.longitude() - deltaX);
    }
}

GeoDataLatLonBox GeoDataLatLonBox::scaled(qreal verticalFactor, qreal horizontalFactor) const
{
    GeoDataLatLonBox result = *this;
    result.scale(verticalFactor, horizontalFactor);
    return result;
}

qreal GeoDataLatLonBox::width(GeoDataCoordinates::Unit unit) const
{
    return GeoDataLatLonBox::width(d->m_east, d->m_west, unit);
}

qreal GeoDataLatLonBox::width(qreal east, qreal west, GeoDataCoordinates::Unit unit)
{
    qreal width = fabs((qreal)(GeoDataLatLonBox::crossesDateLine(east, west) ? 2 * M_PI - west + east : east - west));

    // This also covers the case where this bounding box covers the whole
    // longitude range ( -180 <= lon <= + 180 ).
    if (width > 2 * M_PI) {
        width = 2 * M_PI;
    }

    if (unit == GeoDataCoordinates::Degree) {
        return width * RAD2DEG;
    }

    return width;
}

qreal GeoDataLatLonBox::height(GeoDataCoordinates::Unit unit) const
{
    return GeoDataLatLonBox::height(d->m_north, d->m_south, unit);
}

qreal GeoDataLatLonBox::height(qreal north, qreal south, GeoDataCoordinates::Unit unit)
{
    qreal height = fabs((qreal)(south - north));

    if (unit == GeoDataCoordinates::Degree) {
        return height * RAD2DEG;
    }

    return height;
}

bool GeoDataLatLonBox::crossesDateLine() const
{
    return GeoDataLatLonBox::crossesDateLine(d->m_east, d->m_west);
}

bool GeoDataLatLonBox::crossesDateLine(qreal east, qreal west)
{
    return east < west || (east == M_PI && west == -M_PI);
}

GeoDataCoordinates GeoDataLatLonBox::center() const
{
    if (isEmpty())
        return {};

    if (crossesDateLine())
        return {GeoDataCoordinates::normalizeLon(east() + 2 * M_PI - (east() + 2 * M_PI - west()) / 2), north() - (north() - south()) / 2};
    else
        return {east() - (east() - west()) / 2, north() - (north() - south()) / 2};
}

bool GeoDataLatLonBox::containsPole(Pole pole) const
{
    switch (pole) {
    case NorthPole:
        return (2 * north() == +M_PI);
    case SouthPole:
        return (2 * south() == -M_PI);
    default:
    case AnyPole:
        return (2 * north() == +M_PI || 2 * south() == -M_PI);
    }

    mDebug() << "Invalid pole";
    return false;
}

bool GeoDataLatLonBox::contains(qreal lon, qreal lat) const
{
    if (lat < d->m_south || lat > d->m_north) {
        return false;
    }

    // We need to take care of the normal case ...
    if (((lon < d->m_west || lon > d->m_east) && (d->m_west < d->m_east)) ||
        // ... and the case where the bounding box crosses the date line:
        ((lon < d->m_west && lon > d->m_east) && (d->m_west > d->m_east)))
        return false;

    return true;
}

bool GeoDataLatLonBox::contains(const GeoDataCoordinates &point) const
{
    qreal lon, lat;

    point.geoCoordinates(lon, lat);

    return contains(lon, lat);
}

bool GeoDataLatLonBox::contains(const GeoDataLatLonBox &other) const
{
    // check the contain criterion for the latitude first as this is trivial:

    if (d->m_north >= other.north() && d->m_south <= other.south()) {
        if (!crossesDateLine()) {
            if (!other.crossesDateLine()) {
                // "Normal" case: both bounding boxes don't cross the date line
                if (d->m_west <= other.west() && d->m_east >= other.east()) {
                    return true;
                }
            } else {
                // The other bounding box crosses the date line, "this" one does not:
                // So the date line splits the other bounding box in two parts.
                // Hence "this" bounding box could be fully contained by one of them.
                // So for both cases we are able to ignore the "overhanging" portion
                // and thereby basically reduce the problem to the "normal" case:

                if ((other.west() <= d->m_west && d->m_east <= +M_PI) || (other.east() >= d->m_east && d->m_west >= -M_PI)) {
                    return true;
                }
            }
        } else {
            if (other.crossesDateLine()) {
                // Other "Simple" case: both bounding boxes cross the date line
                if (d->m_west <= other.west() && d->m_east >= other.east()) {
                    return true;
                }
            } else {
                // "This" bounding box crosses the date line, the other one does not.
                // So the date line splits "this" bounding box in two parts.
                // Hence the other bounding box could be fully contained by one of them.
                // So for both cases we are able to ignore the "overhanging" portion
                // and thereby basically reduce the problem to the "normal" case:

                if ((d->m_west <= other.west() && other.east() <= +M_PI) || (d->m_east >= other.east() && other.west() >= -M_PI)) {
                    return true;
                }

                // if this bounding box covers the whole longitude range  ( -180 <= lon <= + 180 )
                // then of course the "inner" bounding box is "inside"
                if (d->m_west == -M_PI && d->m_east == +M_PI) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool GeoDataLatLonBox::intersects(const GeoDataLatLonBox &other) const
{
    if (isEmpty() || other.isEmpty()) {
        return false;
    }

    // check the intersection criterion for the latitude first:

    // Case 1: northern boundary of other box intersects:
    if ((d->m_north >= other.d->m_north && d->m_south <= other.d->m_north)
        // Case 2: northern boundary of this box intersects:
        || (other.d->m_north >= d->m_north && other.d->m_south <= d->m_north)
        // Case 3: southern boundary of other box intersects:
        || (d->m_north >= other.d->m_south && d->m_south <= other.d->m_south)
        // Case 4: southern boundary of this box intersects:
        || (other.d->m_north >= d->m_south && other.d->m_south <= d->m_south)) {
        if (!crossesDateLine()) {
            if (!other.crossesDateLine()) {
                // "Normal" case: both bounding boxes don't cross the date line
                // Case 1: eastern boundary of other box intersects:
                if ((d->m_east >= other.d->m_east && d->m_west <= other.d->m_east)
                    // Case 2: eastern boundary of this box intersects:
                    || (other.d->m_east >= d->m_east && other.d->m_west <= d->m_east)
                    // Case 3: western boundary of other box intersects:
                    || (d->m_east >= other.d->m_west && d->m_west <= other.d->m_west)
                    // Case 4: western boundary of this box intersects:
                    || (other.d->m_east >= d->m_west && other.d->m_west <= d->m_west)) {
                    return true;
                }
            } else {
                // The other bounding box crosses the date line, "this" one does not:
                // So the date line splits the other bounding box in two parts.

                if (d->m_west <= other.d->m_east || d->m_east >= other.d->m_west) {
                    return true;
                }
            }
        } else {
            if (other.crossesDateLine()) {
                // The trivial case: both bounding boxes cross the date line and intersect
                return true;
            } else {
                // "This" bounding box crosses the date line, the other one does not.
                // So the date line splits "this" bounding box in two parts.
                //
                // This also covers the case where this bounding box covers the whole
                // longitude range ( -180 <= lon <= + 180 ).
                if (other.d->m_west <= d->m_east || other.d->m_east >= d->m_west) {
                    return true;
                }
            }
        }
    }

    return false;
}

GeoDataLatLonBox GeoDataLatLonBox::united(const GeoDataLatLonBox &other) const
{
    if (isEmpty()) {
        return other;
    }

    if (other.isEmpty()) {
        return *this;
    }

    GeoDataLatLonBox result;

    // use the position of the centers of the boxes to determine the "smallest"
    // box (i.e. should the total box go through IDL or not). this
    // determination does not depend on one box or the other crossing IDL too
    GeoDataCoordinates c1 = center();
    GeoDataCoordinates c2 = other.center();

    // do latitude first, quite simple
    result.setNorth(qMax(d->m_north, other.north()));
    result.setSouth(qMin(d->m_south, other.south()));

    qreal w1 = d->m_west;
    qreal w2 = other.west();
    qreal e1 = d->m_east;
    qreal e2 = other.east();

    bool const idl1 = d->m_east < d->m_west;
    bool const idl2 = other.d->m_east < other.d->m_west;

    if (idl1) {
        w1 += 2 * M_PI;
        e1 += 2 * M_PI;
    }
    if (idl2) {
        w2 += 2 * M_PI;
        e2 += 2 * M_PI;
    }

    // in the usual case, we take the maximum of east bounds, and
    // the minimum of west bounds. The exceptions are:
    // - centers of boxes are more than 180 apart
    //    (so the smallest box should go around the IDL)
    //
    // - 1 but not 2 boxes are crossing IDL
    if (fabs(c2.longitude() - c1.longitude()) > M_PI || (idl1 ^ idl2)) {
        // exceptions, we go the unusual way:
        // min of east, max of west
        result.setEast(qMin(e1, e2));
        result.setWest(qMax(w1, w2));
    } else {
        // normal case, max of east, min of west
        result.setEast(qMax(e1, e2));
        result.setWest(qMin(w1, w2));
    }
    return result;
}

GeoDataLatLonBox GeoDataLatLonBox::toCircumscribedRectangle() const
{
    QList<GeoDataCoordinates> coordinates;
    coordinates.reserve(4);

    coordinates.append(GeoDataCoordinates(west(), north()));
    coordinates.append(GeoDataCoordinates(west(), south()));
    coordinates.append(GeoDataCoordinates(east() + (crossesDateLine() ? 2 * M_PI : 0), north()));
    coordinates.append(GeoDataCoordinates(east() + (crossesDateLine() ? 2 * M_PI : 0), south()));

    const qreal cosRotation = cos(rotation());
    const qreal sinRotation = sin(rotation());

    qreal centerLat = center().latitude();
    qreal centerLon = center().longitude();
    if (GeoDataLatLonBox(0, 0, center().longitude(), west()).crossesDateLine()) {
        if (!centerLon)
            centerLon += M_PI;
        else
            centerLon += 2 * M_PI;
    }

    GeoDataLatLonBox box;

    bool northSet = false;
    bool southSet = false;
    bool eastSet = false;
    bool westSet = false;

    for (const GeoDataCoordinates &coord : coordinates) {
        const qreal lon = coord.longitude();
        const qreal lat = coord.latitude();

        const qreal rotatedLon = (lon - centerLon) * cosRotation - (lat - centerLat) * sinRotation + centerLon;
        const qreal rotatedLat = (lon - centerLon) * sinRotation + (lat - centerLat) * cosRotation + centerLat;

        if (!northSet || rotatedLat > box.north()) {
            northSet = true;
            box.setNorth(rotatedLat);
        }

        if (!southSet || rotatedLat < box.south()) {
            southSet = true;
            box.setSouth(rotatedLat);
        }

        if (!westSet || rotatedLon < box.west()) {
            westSet = true;
            box.setWest(rotatedLon);
        }

        if (!eastSet || rotatedLon > box.east()) {
            eastSet = true;
            box.setEast(rotatedLon);
        }
    }

    box.setBoundaries(box.north(), box.south(), box.east(), box.west());

    return box;
}

GeoDataLatLonBox &GeoDataLatLonBox::operator=(const GeoDataLatLonBox &other)
{
    GeoDataObject::operator=(other);

    *d = *other.d;
    return *this;
}

GeoDataLatLonBox GeoDataLatLonBox::operator|(const GeoDataLatLonBox &other) const
{
    return united(other);
}

GeoDataLatLonBox &GeoDataLatLonBox::operator|=(const GeoDataLatLonBox &other)
{
    *this = united(other);
    return *this;
}

void GeoDataLatLonBox::pack(QDataStream &stream) const
{
    GeoDataObject::pack(stream);

    stream << d->m_north << d->m_south << d->m_east << d->m_west << d->m_rotation;
}

void GeoDataLatLonBox::unpack(QDataStream &stream)
{
    GeoDataObject::unpack(stream);

    stream >> d->m_north >> d->m_south >> d->m_east >> d->m_west >> d->m_rotation;
}

GeoDataLatLonBox GeoDataLatLonBox::fromLineString(const GeoDataLineString &lineString)
{
    // If the line string is empty return an empty boundingbox
    if (lineString.isEmpty()) {
        return {};
    }

    qreal lon, lat;
    lineString.first().geoCoordinates(lon, lat);
    GeoDataCoordinates::normalizeLonLat(lon, lat);

    qreal north = lat;
    qreal south = lat;
    qreal west = lon;
    qreal east = lon;

    // If there's only a single node stored then the boundingbox only contains that point
    if (lineString.size() == 1)
        return {north, south, east, west};

    // Specifies whether the polygon crosses the IDL
    bool idlCrossed = false;

    // "idlCrossState" specifies the state concerning IDL crossage.
    // This is needed in order to create optimal bounding boxes in case of covering the IDL
    // Every time the IDL gets crossed from east to west the idlCrossState value gets
    // increased by one.
    // Every time the IDL gets crossed from west to east the idlCrossState value gets
    // decreased by one.

    int idlCrossState = 0;
    int idlMaxCrossState = 0;
    int idlMinCrossState = 0;

    // Holds values for east and west while idlCrossState != 0
    qreal otherWest = lon;
    qreal otherEast = lon;

    qreal previousLon = lon;

    int currentSign = (lon < 0) ? -1 : +1;
    int previousSign = currentSign;

    QList<GeoDataCoordinates>::ConstIterator it(lineString.constBegin());
    QList<GeoDataCoordinates>::ConstIterator itEnd(lineString.constEnd());

    bool processingLastNode = false;

    while (it != itEnd) {
        // Get coordinates and normalize them to the desired range.
        (it)->geoCoordinates(lon, lat);
        GeoDataCoordinates::normalizeLonLat(lon, lat);

        // Determining the maximum and minimum latitude
        if (lat > north) {
            north = lat;
        } else if (lat < south) {
            south = lat;
        }

        currentSign = (lon < 0) ? -1 : +1;

        // Once the polyline crosses the dateline the covered bounding box
        // would cover the whole [-M_PI; M_PI] range.
        // When looking separately at the longitude range that gets covered
        // east and west from the IDL we get two bounding boxes (we prefix
        // the resulting longitude range on the "other side" with "other").
        // By picking the "inner" range values we get a more appropriate
        // optimized single bounding box.

        // IDL check
        if (previousSign != currentSign && fabs(previousLon) + fabs(lon) > M_PI) {
            // Initialize values for otherWest and otherEast
            if (idlCrossed == false) {
                otherWest = lon;
                otherEast = lon;
                idlCrossed = true;
            }

            // Determine the new IDL Cross State
            if (previousLon < 0) {
                idlCrossState++;
                if (idlCrossState > idlMaxCrossState) {
                    idlMaxCrossState = idlCrossState;
                }
            } else {
                idlCrossState--;
                if (idlCrossState < idlMinCrossState) {
                    idlMinCrossState = idlCrossState;
                }
            }
        }

        if (idlCrossState == 0) {
            if (lon > east)
                east = lon;
            if (lon < west)
                west = lon;
        } else {
            if (lon > otherEast)
                otherEast = lon;
            if (lon < otherWest)
                otherWest = lon;
        }

        previousLon = lon;
        previousSign = currentSign;

        if (processingLastNode) {
            break;
        }
        ++it;

        if (lineString.isClosed() && it == itEnd) {
            it = lineString.constBegin();
            processingLastNode = true;
        }
    }

    if (idlCrossed) {
        if (idlMinCrossState < 0) {
            east = otherEast;
        }
        if (idlMaxCrossState > 0) {
            west = otherWest;
        }
        if ((idlMinCrossState < 0 && idlMaxCrossState > 0) || idlMinCrossState < -1 || idlMaxCrossState > 1 || west <= east) {
            east = +M_PI;
            west = -M_PI;
            // if polygon fully in south hemisphere, contain south pole
            if (north < 0) {
                south = -M_PI / 2;
            } else {
                north = M_PI / 2;
            }
        }
    }

    return {north, south, east, west};
}

bool GeoDataLatLonBox::isNull() const
{
    return d->m_north == d->m_south && d->m_east == d->m_west;
}

bool GeoDataLatLonBox::isEmpty() const
{
    return *this == empty;
}

bool GeoDataLatLonBox::fuzzyCompare(const GeoDataLatLonBox &lhs, const GeoDataLatLonBox &rhs, const qreal factor)
{
    bool equal = true;

    // Check the latitude for approximate equality

    double latDelta = lhs.height() * factor;

    if (fabs(lhs.north() - rhs.north()) > latDelta)
        equal = false;
    if (fabs(lhs.south() - rhs.south()) > latDelta)
        equal = false;

    // Check the longitude for approximate equality

    double lonDelta = lhs.width() * factor;

    double lhsEast = lhs.east();
    double rhsEast = rhs.east();

    if (!GeoDataLatLonBox::crossesDateLine(lhsEast, rhsEast)) {
        if (fabs(lhsEast - rhsEast) > lonDelta)
            equal = false;
    } else {
        if (lhsEast < 0 && rhsEast > 0) {
            lhsEast += 2 * M_PI;
            if (fabs(lhsEast - rhsEast) > lonDelta)
                equal = false;
        }
        if (lhsEast > 0 && rhsEast < 0) {
            rhsEast += 2 * M_PI;
            if (fabs(lhsEast - rhsEast) > lonDelta)
                equal = false;
        }
    }

    double lhsWest = lhs.west();
    double rhsWest = rhs.west();

    if (!GeoDataLatLonBox::crossesDateLine(lhsWest, rhsWest)) {
        if (fabs(lhsWest - rhsWest) > lonDelta)
            equal = false;
    } else {
        if (lhsWest < 0 && rhsWest > 0) {
            lhsWest += 2 * M_PI;
            if (fabs(lhsWest - rhsWest) > lonDelta)
                equal = false;
        }
        if (lhsWest > 0 && rhsWest < 0) {
            rhsWest += 2 * M_PI;
            if (fabs(lhsWest - rhsWest) > lonDelta)
                equal = false;
        }
    }

    return equal;
}

void GeoDataLatLonBox::clear()
{
    *this = empty;
}
}
