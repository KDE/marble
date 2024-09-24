// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_COORDINATE_H
#define MARBLE_DECLARATIVE_COORDINATE_H

#include "GeoDataCoordinates.h"
#include <QObject>
#include <qqmlregistration.h>

/**
 * Represents a coordinate with the properties of a name and coordinates
 *
 * @todo: Introduce GeoDataCoordinates
 */
class Coordinate : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(qreal longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged)
    Q_PROPERTY(qreal latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(qreal altitude READ altitude WRITE setAltitude NOTIFY altitudeChanged)

    Q_PROPERTY(Notation defaultNotation READ defaultNotation WRITE setDefaultNotation NOTIFY defaultNotationChanged)

public:
    enum Notation {
        Decimal, ///< "Decimal" notation (base-10)
        DMS, ///< "Sexagesimal DMS" notation (base-60)
        DM, ///< "Sexagesimal DM" notation (base-60)
        UTM,
        Astro /// < "RA and DEC" notation (used for astronomical sky coordinates)
    };
    Q_ENUM(Notation)

    /** Constructor */
    explicit Coordinate(qreal lon = 0.0, qreal lat = 0.0, qreal altitude = 0.0, QObject *parent = nullptr);
    explicit Coordinate(const Marble::GeoDataCoordinates &coordinates);

    /** Provides access to the longitude (degree) of the coordinate */
    qreal longitude() const;

    /** Change the longitude of the coordinate */
    void setLongitude(qreal lon);

    /** Provides access to the latitude (degree) of the coordinate */
    qreal latitude() const;

    /** Change the latitude of the coordinate */
    void setLatitude(qreal lat);

    /** Provides access to the altitude (meters) of the coordinate */
    qreal altitude() const;

    /** Change the altitude of the coordinate */
    void setAltitude(qreal alt);

    /** Change the altitude of the coordinate */
    Marble::GeoDataCoordinates coordinates() const;

    /** Change all coordinates at once */
    void setCoordinates(const Marble::GeoDataCoordinates &coordinates);

    Q_INVOKABLE QString toGeoString(Coordinate::Notation notation = Coordinate::DMS, int precision = -1) const;

    /** Distance (in meter) to the given coordinate */
    Q_INVOKABLE qreal distance(qreal longitude, qreal latitude) const;

    /** Bearing (in degree) to the given coordinate */
    Q_INVOKABLE qreal bearing(qreal longitude, qreal latitude) const;

    bool operator==(const Coordinate &other) const;

    bool operator!=(const Coordinate &other) const;

    Notation defaultNotation();
    void setDefaultNotation(Notation defaultNotation);

Q_SIGNALS:
    void longitudeChanged();
    void latitudeChanged();
    void altitudeChanged();

    void defaultNotationChanged(Notation defaultNotation);

private:
    Marble::GeoDataCoordinates m_coordinate;
    Notation m_defaultNotation;
};

#endif // MARBLE_DECLARATIVE_COORDINATE_H
