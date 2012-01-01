//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_OSMGLOBALS_H
#define MARBLE_OSMGLOBALS_H

#include <QtCore/QMap>
#include <QtGui/QColor>
#include <QtCore/QString>
#include <QtCore/QList>
#include "GeoDataFeature.h"

namespace Marble
{
class GeoDataStyle;
class GeoDataPlacemark;

namespace osm
{

class OsmGlobals
{
public:
    static bool tagNeedArea( const QString& keyValue );
    static void addDummyPlacemark( GeoDataPlacemark *placemark );
    static void cleanUpDummyPlacemarks();

    static QColor buildingColor;
    static QColor backgroundColor;

private:
    static void setupCategories();
    static void setupAreaTags();
    
    static QList<QString> m_areaTags;
    static QList<GeoDataPlacemark*> dummyPlacemarks;
};

}
}

#endif // MARBLE_OSMGLOBALS_H

