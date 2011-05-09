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

namespace osm
{

class OsmGlobals
{
public:
    static QMap<QString, GeoDataFeature::GeoDataVisualCategory> visualCategories();
    static bool tagNeedArea( const QString& keyValue );

    static QColor buildingColor;
    static QColor backgroundColor;

private:
    static void setupCategories();
    static void setupAreaTags();
    
    static QMap<QString, GeoDataFeature::GeoDataVisualCategory> m_visualCategories;
    static QList<QString> m_areaTags;
};

}
}

#endif // MARBLE_OSMGLOBALS_H

