//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_OSMPLACEMARK_H
#define MARBLE_OSMPLACEMARK_H

#include <QtCore/QString>

namespace Marble {

/**
  * A lightweight data structure to represent points of interest
  * like addresses with support for serialization.
  */
class OsmPlacemark
{
public:
    OsmPlacemark();

    /** Placemark name */
    QString name() const;

    void setName( const QString &name );

    /** Identifier of the smallest region containing this placemark,
        -1 if none. */
    int regionId() const;

    void setOsmRegionId( int id );

    /** Longitude of the placemark's center point, in degree */
    qreal longitude() const;

    void setLongitude( qreal longitude );

    /** Latitude of the placemark's center point, in degree */
    qreal latitude() const;

    void setLatitude( qreal latitude );

    /** Placemarks are sorted by name by default */
    bool operator<( const OsmPlacemark &other) const;

private:
    int m_regionId;

    QString m_name;

    qreal m_longitude;

    qreal m_latitude;
};

}

QDataStream& operator<<( QDataStream& out, const Marble::OsmPlacemark& region );

QDataStream& operator>>( QDataStream& out, Marble::OsmPlacemark& region );

#endif // MARBLE_OSMPLACEMARK_H
