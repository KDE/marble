/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DgmlSourceDirTagHandler.h"

#include "DgmlElementDictionary.h"
#include "DgmlAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"

namespace Marble
{
namespace dgml
{
DGML_DEFINE_TAG_HANDLER(SourceDir)

GeoNode* DgmlSourceDirTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(dgmlTag_SourceDir)));

    QString format = parser.attribute(dgmlAttr_format).trimmed();

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Texture) || parentItem.represents(dgmlTag_Vectortile) ) {
        GeoSceneTileDataset *texture = parentItem.nodeAs<GeoSceneTileDataset>();
        texture->setSourceDir( parser.readElementText().trimmed() );
        texture->setFileFormat(format);
    }

    return nullptr;
}

}
}
