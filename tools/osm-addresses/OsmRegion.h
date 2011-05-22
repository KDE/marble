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

#include "GeoDataPolygon.h"

#include <QtCore/QList>
#include <QtCore/QString>

namespace Marble
{

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

    int parentIdentifier() const;

    void setParentIdentifier( int identifier );

    QString name() const;

    void setName( const QString &name );

    /** Longitude of the region's center point, in degree */
    qreal longitude() const;

    void setLongitude( qreal longitude );

    /** Latitude of the region's center point, in degree */
    qreal latitude() const;

    void setLeft( int left );

    int left() const;

    void setRight( int right );

    int right() const;

    void setLatitude( qreal latitude );

    bool operator==( const OsmRegion &other ) const;

    const GeoDataPolygon& geometry() const;

    void setGeometry( const GeoDataPolygon &polygon );

    int adminLevel() const;

    void setAdminLevel( int level );

private:
    static int m_idFactory;

    int m_identifier;

    int m_parent;

    QString m_name;

    qreal m_longitude;

    qreal m_latitude;

    int m_left;

    int m_right;

    GeoDataPolygon m_geometry;

    int m_adminLevel;
};

}

#endif // MARBLE_OSMREGION_H
