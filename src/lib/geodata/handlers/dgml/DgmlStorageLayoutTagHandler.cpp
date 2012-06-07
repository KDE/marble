/*
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "DgmlStorageLayoutTagHandler.h"

#include "MarbleDebug.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTiled.h"
#include "ServerLayout.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(StorageLayout)

GeoNode* DgmlStorageLayoutTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_StorageLayout));

    // Attribute levelZeroColumns, default to value of the oldest tile themes
    int levelZeroColumns = 2;
    const QString levelZeroColumnsStr = parser.attribute(dgmlAttr_levelZeroColumns).trimmed();
    if ( !levelZeroColumnsStr.isEmpty() ) {
        levelZeroColumns = levelZeroColumnsStr.toInt();
    }

    // Attribute levelZeroRows, default to value of the oldest tile themes
    int levelZeroRows = 1;
    const QString levelZeroRowsStr = parser.attribute(dgmlAttr_levelZeroRows).trimmed();
    if ( !levelZeroRowsStr.isEmpty() ) {
        levelZeroRows = levelZeroRowsStr.toInt();
    }

    // Attribute maximumTileLevel
    int maximumTileLevel = -1;
    const QString maximumTileLevelStr = parser.attribute( dgmlAttr_maximumTileLevel ).trimmed();
    if ( !maximumTileLevelStr.isEmpty() ) {
        maximumTileLevel = maximumTileLevelStr.toInt();
    }

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Texture) || parentItem.represents(dgmlTag_VectorTile)) {
        GeoSceneTiled *texture = parentItem.nodeAs<GeoSceneTiled>();

        // Attribute mode
        GeoSceneTiled::StorageLayout storageLayout = GeoSceneTiled::OpenStreetMap;
        ServerLayout *serverLayout = 0;
        const QString modeStr = parser.attribute(dgmlAttr_mode).trimmed();
        if ( modeStr == "OpenStreetMap" )
            serverLayout = new OsmServerLayout( texture );
        else if ( modeStr == "Custom" )
            serverLayout = new CustomServerLayout( texture );
        else if ( modeStr == "WebMapService" )
            serverLayout = new WmsServerLayout( texture );
        else if ( modeStr == "QuadTree" )
            serverLayout = new QuadTreeServerLayout( texture );
        else {
            storageLayout = GeoSceneTiled::Marble;
            serverLayout = new MarbleServerLayout( texture );
        }

        texture->setLevelZeroColumns( levelZeroColumns );
        texture->setLevelZeroRows( levelZeroRows );
        texture->setMaximumTileLevel( maximumTileLevel );
        texture->setStorageLayout( storageLayout );
        texture->setServerLayout( serverLayout );
    }

    return 0;
}

}
}
