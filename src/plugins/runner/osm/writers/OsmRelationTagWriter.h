//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMRELATIONTAGWRITER_H
#define MARBLE_OSMRELATIONTAGWRITER_H


#include <QMap>

namespace Marble
{

class GeoDataPolygon;
class GeoWriter;
class GeoDataPlacemark;
class OsmPlacemarkData;

class OsmRelationTagWriter
{

public:
    static void writeRelation( const QVector<GeoDataPlacemark*>& relation, const QString& id,
                           const QMap<QString,QString>& tags, GeoWriter& writer );

    static void writeMultipolygon( const GeoDataPolygon& polygon,
                                   const OsmPlacemarkData& osmData, GeoWriter& writer );
};

}

#endif



