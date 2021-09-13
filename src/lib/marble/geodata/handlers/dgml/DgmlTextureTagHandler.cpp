/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlTextureTagHandler.h"

#include <limits>

#include "MarbleDebug.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "DgmlAuxillaryDictionary.h"
#include "GeoParser.h"
#include "GeoSceneLayer.h"
#include "GeoSceneTextureTileDataset.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(Texture)

GeoNode* DgmlTextureTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_Texture)));

    const QString name = parser.attribute(dgmlAttr_name).trimmed();

    const QString expireStr = parser.attribute(dgmlAttr_expire).trimmed();
    int expire = std::numeric_limits<int>::max();
    if ( !expireStr.isEmpty() )
        expire = expireStr.toInt();

    GeoSceneTileDataset *texture = nullptr;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();

    // Check parent type and make sure that the dataSet type 
    // matches the backend of the parent layer
    if ( parentItem.represents(dgmlTag_Layer)
        && parentItem.nodeAs<GeoSceneLayer>()->backend() == dgmlValue_texture ) {

        texture = new GeoSceneTextureTileDataset( name );
        texture->setExpire( expire );
        parentItem.nodeAs<GeoSceneLayer>()->addDataset( texture );
    }

    return texture;
}

}
}
