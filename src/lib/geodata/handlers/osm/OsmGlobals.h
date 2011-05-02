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
#include <root/include/marble/GeoDataFeature.h>

namespace Marble
{
class GeoDataStyle;

namespace osm
{

class OsmGlobals
{
public:
    static QMap<QString, GeoDataStyle*> poiStyles();
    static QMap<QString, GeoDataFeature::GeoDataVisualCategory> visualCategories();

    static QColor buildingColor;
    static QColor backgroundColor;
    static QColor waterColor;

private:
    static void appendStyle( const QString& name, const QString& icon );
    static void setupPOIStyles();
    static void setupCategories();

    static QMap<QString, GeoDataStyle*> m_poiStyles;
    static QMap<QString, GeoDataFeature::GeoDataVisualCategory> m_visualCategories;
};

}
}

#endif // MARBLE_OSMGLOBALS_H

