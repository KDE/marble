/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlOpenTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataFeature.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(open)

GeoNode *KmlopenTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_open)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>()) {
        QString open = parser.readElementText().trimmed();
        /*        if (open == QLatin1StringView("1"))
                    parentItem.nodeAs<GeoDataFeature>()->setOpen( true );
                else
                    parentItem.nodeAs<GeoDataFeature>()->setOpen( false );*/
    }

    return nullptr;
}

}
}
