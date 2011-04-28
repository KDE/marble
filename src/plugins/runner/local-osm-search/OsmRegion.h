//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_OSMREGION_H
#define MARBLE_OSMREGION_H

#include <QtCore/QList>
#include <QtCore/QString>

namespace Marble {

/**
  * A lightweight data structure to represent administrative regions
  * like villages, cities, states, ... with support for serialization.
  */
class OsmRegion
{
public:
    OsmRegion();

    /** Unique (per process) region identifier */
    int identifier() const;

    /** Identifier change. Usage is only intended for
      * serialization; newly created regions get an
      * identifier automatically
      */
    void setIdentifier( int identifier );

    QString name() const;

    void setName( const QString &name );

    /** Longitude of the region's center point, in degree */
    qreal longitude() const;

    void setLongitude( qreal longitude );

    /** Latitude of the region's center point, in degree */
    qreal latitude() const;

    void setLatitude( qreal latitude );

private:
    static int m_idFactory;

    int m_identifier;

    QString m_name;

    qreal m_longitude;

    qreal m_latitude;
};

}

QDataStream& operator<<( QDataStream& out, const Marble::OsmRegion& region );

QDataStream& operator>>( QDataStream& out, Marble::OsmRegion& region );

#endif // MARBLE_OSMREGION_H
