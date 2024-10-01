// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2021 Torsten Rahn
//

#include "Coordinate.h"

#include "MarbleGlobal.h"

using Marble::DEG2RAD;
using Marble::EARTH_RADIUS;
using Marble::GeoDataCoordinates;

Coordinate::Coordinate(qreal lon, qreal lat, qreal alt, QObject *parent)
    : QObject(parent)
{
    setLongitude(lon);
    setLatitude(lat);
    setAltitude(alt);
}

Coordinate::Coordinate(const Marble::GeoDataCoordinates &coordinates)
{
    setCoordinates(coordinates);
}

qreal Coordinate::longitude() const
{
    return m_coordinate.longitude(GeoDataCoordinates::Degree);
}

void Coordinate::setLongitude(qreal lon)
{
    m_coordinate.setLongitude(lon, GeoDataCoordinates::Degree);
    Q_EMIT longitudeChanged();
}

qreal Coordinate::latitude() const
{
    return m_coordinate.latitude(GeoDataCoordinates::Degree);
}

void Coordinate::setLatitude(qreal lat)
{
    m_coordinate.setLatitude(lat, GeoDataCoordinates::Degree);
    Q_EMIT latitudeChanged();
}

qreal Coordinate::altitude() const
{
    return m_coordinate.altitude();
}

void Coordinate::setAltitude(qreal alt)
{
    m_coordinate.setAltitude(alt);
    Q_EMIT altitudeChanged();
}

GeoDataCoordinates Coordinate::coordinates() const
{
    return m_coordinate;
}

void Coordinate::setCoordinates(const GeoDataCoordinates &coordinates)
{
    m_coordinate = coordinates;
}

QString Coordinate::toGeoString(Coordinate::Notation notation, int precision) const
{
    return m_coordinate.toString(static_cast<GeoDataCoordinates::Notation>(notation), precision);
}

qreal Coordinate::distance(qreal longitude, qreal latitude) const
{
    GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
    GeoDataCoordinates other(longitude, latitude, 0, deg);
    return EARTH_RADIUS * coordinates().sphericalDistanceTo(other);
}

qreal Coordinate::bearing(qreal longitude, qreal latitude) const
{
    qreal deltaLon = longitude * DEG2RAD - m_coordinate.longitude();
    qreal y = sin(deltaLon) * cos(latitude * DEG2RAD);
    qreal x = cos(m_coordinate.latitude()) * sin(latitude * DEG2RAD) - sin(m_coordinate.latitude()) * cos(latitude * DEG2RAD) * cos(deltaLon);
    return Marble::RAD2DEG * atan2(y, x);
}

bool Coordinate::operator==(const Coordinate &other) const
{
    return m_coordinate == other.m_coordinate;
}

bool Coordinate::operator!=(const Coordinate &other) const
{
    return !operator==(other);
}

Coordinate::Notation Coordinate::defaultNotation()
{
    return static_cast<Coordinate::Notation>(GeoDataCoordinates::defaultNotation());
}

void Coordinate::setDefaultNotation(Coordinate::Notation defaultNotation)
{
    if (GeoDataCoordinates::defaultNotation() == static_cast<GeoDataCoordinates::Notation>(defaultNotation))
        return;
    GeoDataCoordinates::setDefaultNotation(static_cast<GeoDataCoordinates::Notation>(defaultNotation));
    Q_EMIT defaultNotationChanged(defaultNotation);
}

#include "moc_Coordinate.cpp"
