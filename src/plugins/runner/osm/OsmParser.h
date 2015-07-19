//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
// Copyright 2014      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef OSMPARSER_H
#define OSMPARSER_H

#include "GeoParser.h"
#include "GeoDataCoordinates.h"

#include <QColor>
#include <QList>
#include <QMap>
#include <QSet>

namespace Marble {

class GeoDataLineString;
class GeoDataPlacemark;
class GeoDataPoint;
class GeoDataPolygon;
class OsmPlacemarkData;

class OsmParser : public GeoParser
{
public:
    OsmParser();
    virtual ~OsmParser();

    void setNode( qint64 id, GeoDataPoint *point );
    GeoDataPoint *node( qint64 id );

    void setWay( qint64 id, GeoDataLineString *way );
    GeoDataLineString *way( qint64 id );

    void setPolygon( qint64 id, GeoDataPolygon *polygon );
    GeoDataPolygon *polygon( qint64 id );

    bool tagNeedArea( const QString &keyValue ) const;
    void addDummyPlacemark( GeoDataPlacemark *placemark );

    /**
     * @brief osmAttributeData is a convenience function that parses all osm-related
     * arguments of a tag
     * @return an OsmPlacemarkData object containing all the necessary data
     */
    OsmPlacemarkData osmAttributeData() const;

    static const QColor backgroundColor;

private:
    virtual bool isValidElement(const QString& tagName) const;
    virtual bool isValidRootElement();

    virtual GeoDocument* createDocument() const;

    QMap<qint64, GeoDataPoint *> m_nodes;
    QMap<qint64, GeoDataPolygon *> m_polygons;
    QMap<qint64, GeoDataLineString *> m_ways;
    QSet<QString> m_areaTags;
    QList<GeoDataPlacemark *> m_dummyPlacemarks;
};

}

#endif // OSMPARSER_H
