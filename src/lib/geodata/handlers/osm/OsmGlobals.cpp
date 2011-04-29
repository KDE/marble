//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmGlobals.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "global.h"
#include "MarbleDirs.h"

namespace Marble
{
namespace osm
{
QMap<QString, GeoDataStyle*> OsmGlobals::m_poiStyles;

const QMap< QString, GeoDataStyle* >& OsmGlobals::poiStyles()
{
    if ( m_poiStyles.count() < 1 )
        setupPOIStyles();

    return m_poiStyles;
}

void OsmGlobals::setupPOIStyles()
{
    appendStyle( "amenity=restaurant", "food_restaurant.png" );
}

void OsmGlobals::appendStyle( const QString& name, const QString& icon )
{
    GeoDataIconStyle iconStyle;
    iconStyle.setIconPath( MarbleDirs::path( "bitmaps/poi/" + icon ) );
    GeoDataStyle *style = new GeoDataStyle();
    style->setIconStyle( iconStyle );
    m_poiStyles[name] = style;
}

}
}

