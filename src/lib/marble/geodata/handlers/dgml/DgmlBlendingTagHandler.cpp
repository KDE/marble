// SPDX-FileCopyrightText: 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "DgmlBlendingTagHandler.h"

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTileDataset.h"
#include "MarbleDebug.h"

namespace Marble
{
namespace dgml
{
static GeoTagHandlerRegistrar registrar(GeoParser::QualifiedName(QString::fromLatin1(dgmlTag_Blending), QString::fromLatin1(dgmlTag_nameSpace20)),
                                        new DgmlBlendingTagHandler);

GeoNode *DgmlBlendingTagHandler::parse(GeoParser &parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(dgmlTag_Blending)));

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (!parentItem.represents(dgmlTag_Texture) && !parentItem.represents(dgmlTag_Vectortile))
        return nullptr;

    // Attribute name, default to ""
    const QString name = parser.attribute(dgmlAttr_name).trimmed();
    mDebug() << name;
    parentItem.nodeAs<GeoSceneTileDataset>()->setBlending(name);
    return nullptr;
}

}
}
