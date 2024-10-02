/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlDescriptionTagHandler.h"

#include "KmlElementDictionary.h"

#include "GeoDataFeature.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(description)

GeoNode *KmldescriptionTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_description)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>()) {
        QString result;
        bool finished = false;
        bool isCDATA = false;
        while (!finished) {
            switch (parser.readNext()) {
            case QXmlStreamReader::Characters:
            case QXmlStreamReader::EntityReference: {
                result.append(parser.text());
                if (parser.isCDATA()) {
                    isCDATA = true;
                }
                break;
            }
            case QXmlStreamReader::EndElement:
                finished = true;
                break;
            case QXmlStreamReader::ProcessingInstruction:
            case QXmlStreamReader::Comment:
                break;
            default:
                finished = true;
                break;
            }
        }

        QString description = result.trimmed();

        parentItem.nodeAs<GeoDataFeature>()->setDescription(description);
        parentItem.nodeAs<GeoDataFeature>()->setDescriptionCDATA(isCDATA);
    }

    return nullptr;
}

}
}
