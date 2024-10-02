// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlModelTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataModel.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Model)

GeoNode *KmlModelTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Model)));

    auto model = new GeoDataModel;
    KmlObjectTagHandler::parseIdentifiers(parser, model);

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Placemark)) {
        parentItem.nodeAs<GeoDataPlacemark>()->setGeometry(model);
        return model;

    } else if (parentItem.represents(kmlTag_MultiGeometry)) {
        parentItem.nodeAs<GeoDataMultiGeometry>()->append(model);
        return model;

    } else {
        delete model;
        return nullptr;
    }
}

}
}
