/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#if KML_LAZY_IMP
#include "KmlRoleTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataPlacemark.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(role)

GeoNode *KmlroleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_role)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataPlacemark>()) {
        QString role = parser.readElementText().trimmed();
        if (role.isEmpty()) {
            role = QLatin1Char(' ');
        }
        parentItem.nodeAs<GeoDataPlacemark>()->setRole(role);
    }

    return nullptr;
}

}
}

#endif // KML_LAZY_IMP
