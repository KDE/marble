// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "DgmlTileSizeTagHandler.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(TileSize)

GeoNode* DgmlTileSizeTagHandler::parse( GeoParser& parser ) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_TileSize)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if ( !parentItem.represents( dgmlTag_Texture ) && !parentItem.represents( dgmlTag_Vectortile ))
        return nullptr;

    int width = parser.attribute(dgmlAttr_width).toInt();
    int height = parser.attribute(dgmlAttr_height).toInt();
    QSize const size( width, height );
    if ( !size.isEmpty() ) {
        parentItem.nodeAs<GeoSceneTileDataset>()->setTileSize( size );
    }

    return nullptr;
}

}
}
