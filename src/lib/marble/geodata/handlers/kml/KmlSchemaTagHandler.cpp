/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlSchemaTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataDocument.h"
#include "GeoDataSchema.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Schema)

GeoNode *KmlSchemaTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Schema)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Document)) {
        GeoDataSchema schema;
        KmlObjectTagHandler::parseIdentifiers(parser, &schema);
        QString name = parser.attribute("name").trimmed();

        schema.setSchemaName(name);
        parentItem.nodeAs<GeoDataDocument>()->addSchema(schema);
        return &parentItem.nodeAs<GeoDataDocument>()->schema(schema.id());
    }
    return nullptr;
}

}
}
