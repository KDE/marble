//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_COORDINATE_H
#define MARBLE_DECLARATIVE_COORDINATE_H

#include "GeoDataCoordinates.h"

#include <QtCore/QObject>
#include <QtDeclarative/QtDeclarative>

/**
  * Represents a coordinate with the properties of a name and coordinates
  *
  * @todo: Introduce GeoDataCoordinates
  */
class Coordinate : public QObject
{
    Q_OBJECT

    Q_PROPERTY( qreal longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged )
    Q_PROPERTY( qreal latitude  READ latitude  WRITE setLatitude  NOTIFY latitudeChanged )
    Q_PROPERTY( qreal altitude  READ altitude  WRITE setAltitude  NOTIFY altitudeChanged )

public:
    /** Constructor */
    explicit Coordinate( qreal lon = 0.0, qreal lat = 0.0, qreal altitude = 0.0, QObject *parent = 0 );

    /** Provides access to the longitude (degree) of the coordinate */
    qreal longitude() const;

    /** Change the longitude of the coordinate */
    void setLongitude( qreal lon );

    /** Provides access to the latitude (degree) of the coordinate */
    qreal latitude() const;

    /** Change the latitude of the coordinate */
    void setLatitude( qreal lat );

    /** Provides access to the altitude (meters) of the coordinate */
    qreal altitude() const;

    /** Change the altitude of the coordinate */
    void setAltitude( qreal alt );

    /** Change the altitude of the coordinate */
    Marble::GeoDataCoordinates coordinates() const;

    /** Change all coordinates at once */
    void setCoordinates( const Marble::GeoDataCoordinates &coordinates );

    /** Distance (in meter) to the given coordinate */
    Q_INVOKABLE qreal distance( qreal longitude, qreal latitude ) const;

    /** Bearing (in degree) to the given coordinate */
    Q_INVOKABLE qreal bearing( qreal longitude, qreal latitude ) const;

    bool operator == ( const Coordinate &other ) const;

    bool operator != ( const Coordinate &other ) const;

Q_SIGNALS:
    void longitudeChanged();
    void latitudeChanged();
    void altitudeChanged();

private:
    Marble::GeoDataCoordinates m_coordinate;
};

QML_DECLARE_TYPE( Coordinate )

#endif // MARBLE_DECLARATIVE_COORDINATE_H
