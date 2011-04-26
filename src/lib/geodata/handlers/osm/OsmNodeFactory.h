//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_OSMNODEFACTORY_H
#define MARBLE_OSMNODEFACTORY_H

#include <QMap>

namespace Marble
{

class GeoDataPoint;
  
namespace osm
{
  
class OsmNodeFactory
{
public:
    static void appendPoint(quint64 id, GeoDataPoint *p);
    static GeoDataPoint *getPoint(quint64 id);
    
private:
    static QMap<quint64, GeoDataPoint *> m_points;
};

}
}

#endif // MARBLE_OSMNODEFACTORY_H
